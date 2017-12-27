// bgDeviceControl.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "bgDeviceControl.h"
#include "bgDaHuaDeviceControl.h"
#include "bgLogging.h"
#include "json/json.h"

#define COMMAND_PATH		"/goldmsg/car/DeviceControl"
#define STREAM_PATH			"/goldmsg/car/Live.flv"

#define MAX_CMD_LEN				4096

bgDeviceControl::bgDeviceControl()
	: dahua_device_(new bgDahuaDeviceControl())
{

}

bgDeviceControl::~bgDeviceControl()
{
	delete dahua_device_;
	dahua_device_ = nullptr;
}

int bgDeviceControl::Init(const char *config_ini)
{
	int errCode = 0;

	// 为每一个设备调用初始化接口
	errCode = dahua_device_->OnInit(config_ini);

	return errCode;
}

bool bgDeviceControl::IsMyMsg(unsigned long connect_id, const char *method, const char *path)
{
	// 首先判断提交的路径是否是本模块关注的路径
	// 然后判断本模块接受的方法，目前设定接受GET和POST
	if (_stricmp(method, "GET") == 0)
	{
		// GET操作可以用于获取媒体流
		if (_stricmp(path, STREAM_PATH) != 0)
			return false;
		else
			return true;
	}
	else if (_stricmp(method, "POST") == 0)
	{
		// 判断是不是信令处理的路径
		if (_stricmp(path, COMMAND_PATH) != 0)
			return false;
		else
			return true;
	}
	else
		return false;
}

int bgDeviceControl::SetHttpContentLength(unsigned long connect_id, unsigned long long data_len)
{
	int errCode = 0;

	// 本模块默认支持的最大命令字节数为4k
	if (data_len > MAX_CMD_LEN)
		return ERROR_BUFFER_OVERFLOW;

	// 构建命令缓冲，准备接收数据
	COMMAND_DATA cmddata;
	cmddata.cmddata_size_ = 0;
	cmddata.cmddata_buffer_len_ = data_len + 1;
	cmddata.cmddata_ = new unsigned char[cmddata.cmddata_buffer_len_];
	memset(cmddata.cmddata_, 0, cmddata.cmddata_buffer_len_);

	cmds_.insert(std::pair<unsigned long, COMMAND_DATA>(connect_id, cmddata));

	return errCode;
}

int bgDeviceControl::CacheHttpContentData(unsigned long connect_id, const unsigned char *data, int data_len)
{
	int errCode = 0;

	// 找缓冲
	std::map<unsigned long, COMMAND_DATA>::iterator iter = cmds_.find(connect_id);
	if (iter == cmds_.end())
		return ERROR_NOT_FOUND;

	// 缓冲数据
	COMMAND_DATA cmddata = iter->second;
	memcpy(cmddata.cmddata_ + cmddata.cmddata_size_, data, data_len);
	cmddata.cmddata_size_ += data_len;

	return errCode;
}

int bgDeviceControl::HandleRequest(unsigned long connect_id, const char *method, const char *path, unsigned char **response_data, int *response_len, const char *query /* = nullptr */)
{
	char msg[409600] = {0};
	int errCode = 0;

	if (_stricmp("GET", method) == 0)
	{
		if (!IsMyMsg(connect_id, method, path))
			errCode = ERROR_NOT_SUPPORTED;
		else
		{
			// 根据查询条件读取
			// 这里简单粗暴一点吧，直接传数据
			dahua_device_->ReadCacheRealStreamData(response_data, response_len);
			errCode = 1;
		}
	}
	else if (_stricmp("POST", method) == 0)
	{
		if (!IsMyMsg(connect_id, method, path))
			errCode = ERROR_NOT_SUPPORTED;
		else
		{
			// 找到缓冲
			std::map<unsigned long, COMMAND_DATA>::iterator iter = cmds_.find(connect_id);
			if (iter == cmds_.end())
				return ERROR_NOT_FOUND;

			COMMAND_DATA cmddata = iter->second;
			std::string request_data = (const char *)cmddata.cmddata_;

			// 输出缓冲
			sprintf_s(msg, 409600, "Handle request :", request_data.c_str());
			BG_LOG_INFO(msg);

			// 将请求数据转换为json对象进行处理
			// 由于我们这里是服务器程序，要妥善处理异常情况，这里应当使用防御式编程
			Json::Reader reader;
			Json::Value json_root;

			if (!reader.parse(request_data, json_root))
				errCode = ERROR_BAD_COMMAND;
			else
			{
				// 处理返回结果
				Json::Value result;
				
				// 首先检查几个必须有的字段
				// id/commandtype/subcmd/manufacturer
				if (json_root.isMember("id") && json_root.isMember("commandtype") && json_root.isMember("subcmd") && json_root.isMember("manufacturer"))
				{
					result["id"] = Json::Value(json_root["id"].asString());

					if (json_root["commandtype"].asString().compare("PTZControl") == 0)
					{
						// 这是云台控制指令
						std::string subcmd = json_root["subcmd"].asString();
						std::string manufacturer = json_root["manufacturer"].asString();

						// 云台控制命令必须有的参数：state/value/speed
						if (json_root.isMember("state") && json_root.isMember("value") && json_root.isMember("speed"))
						{
							std::string cmdval = json_root["value"].asString();
							std::string cmdstate = json_root["state"].asString();
							int speed = json_root["speed"].asInt();

							// 发送控制指令到设备
							if (manufacturer.compare("dh") == 0)
								errCode = dahua_device_->OnPTZControl(subcmd.c_str(), cmdval.c_str(), 0, speed, 0, cmdstate.compare("Stop") == 0 ? TRUE : FALSE);
							else
								errCode = ERROR_NOT_SUPPORTED;
						}
						else
							errCode = ERROR_BAD_COMMAND;
					}
					else if (json_root["commandtype"].asString().compare("stream") == 0)
					{
						// 这是点流指令，向设备发送点流请求
						std::string subcmd = json_root["subcmd"].asString();
						std::string manufacturer = json_root["manufacturer"].asString();

						
						if (_stricmp("realvideo", subcmd.c_str()) == 0)
						{
							// 实时点流命令，判断是开始点流还是结束点流
							if (json_root.isMember("state"))
							{
								std::string cmdstate = json_root["state"].asString();
								if (_stricmp(cmdstate.c_str(), "Start") == 0)
								{
									// 开始命令，判断是哪个
									if (manufacturer.compare("dh") == 0)
									{
										errCode = dahua_device_->OnStartRealPlay();
										if (errCode == 0)
										{
											result["stream_protocol"] = Json::Value(dahua_device_->GetRealStreamProtocol());
											result["stream_name"] = Json::Value(dahua_device_->GetRealStreamName());
										}
									}
								}
								else
								{
									if (manufacturer.compare("dh") == 0)
										errCode = dahua_device_->OnStopRealPlay();
								}
							}
						}
						else if (_stricmp("query_realvideo_url", subcmd.c_str()) == 0)
						{
							std::string stream_url = dahua_device_->GetRealStreamUrl();
							result["stream_url"] = Json::Value(stream_url);
							errCode = 0;
						}
						else
							errCode = ERROR_BAD_COMMAND;
					}

					if (errCode == 0)
						result["status"] = Json::Value("OK");
					else
						result["status"] = Json::Value("FAIL");

					Json::StyledWriter sw;
					std::string result_data = sw.write(result);

					sprintf_s(msg, 409600, "Response data : %s", result_data.c_str());
					BG_LOG_INFO(msg);

					if (response_len)
					{
						*response_len = result_data.size();
						*response_data = new unsigned char[*response_len + 1];
						memcpy(*response_data, result_data.c_str(), result_data.size());
					}
				}
			}

			// 这里无论处理成功还是失败，我们都应该释放掉这个元素
			delete [] cmddata.cmddata_;
			cmddata.cmddata_ = nullptr;
			cmddata.cmddata_buffer_len_ = 0;
			cmddata.cmddata_size_ = 0;

			// 这里的操作应该是需要上锁的，这里我先不上锁
			cmds_.erase(connect_id);
		}
	}
	else
		errCode = ERROR_NOT_SUPPORTED;

	// 处理完毕之后返回处理结果

	return errCode;
}

void bgDeviceControl::CleanupResponseData(unsigned long connect_id, const char *method, unsigned char **response_data)
{
	if (_stricmp(method, "GET") == 0)
	{
		dahua_device_->ReleaseCacheResource(response_data);
	}
	else
	{
		delete [] *response_data;
		*response_data = nullptr;
	}
}

extern "C" bgHttpBusinessPlugins* __stdcall CreateObject()
{
	return (bgHttpBusinessPlugins*)new bgDeviceControl();
}

extern "C" void __stdcall DestroyObject(bgHttpBusinessPlugins** plugin)
{
	delete *plugin;
	*plugin = NULL;
}