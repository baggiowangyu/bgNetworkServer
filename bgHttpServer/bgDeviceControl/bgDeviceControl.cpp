// bgDeviceControl.cpp : ���� DLL Ӧ�ó���ĵ���������
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

	// Ϊÿһ���豸���ó�ʼ���ӿ�
	errCode = dahua_device_->OnInit(config_ini);

	return errCode;
}

bool bgDeviceControl::IsMyMsg(unsigned long connect_id, const char *method, const char *path)
{
	// �����ж��ύ��·���Ƿ��Ǳ�ģ���ע��·��
	// Ȼ���жϱ�ģ����ܵķ�����Ŀǰ�趨����GET��POST
	if (_stricmp(method, "GET") == 0)
	{
		// GET�����������ڻ�ȡý����
		if (_stricmp(path, STREAM_PATH) != 0)
			return false;
		else
			return true;
	}
	else if (_stricmp(method, "POST") == 0)
	{
		// �ж��ǲ���������·��
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

	// ��ģ��Ĭ��֧�ֵ���������ֽ���Ϊ4k
	if (data_len > MAX_CMD_LEN)
		return ERROR_BUFFER_OVERFLOW;

	// ��������壬׼����������
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

	// �һ���
	std::map<unsigned long, COMMAND_DATA>::iterator iter = cmds_.find(connect_id);
	if (iter == cmds_.end())
		return ERROR_NOT_FOUND;

	// ��������
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
			// ���ݲ�ѯ������ȡ
			// ����򵥴ֱ�һ��ɣ�ֱ�Ӵ�����
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
			// �ҵ�����
			std::map<unsigned long, COMMAND_DATA>::iterator iter = cmds_.find(connect_id);
			if (iter == cmds_.end())
				return ERROR_NOT_FOUND;

			COMMAND_DATA cmddata = iter->second;
			std::string request_data = (const char *)cmddata.cmddata_;

			// �������
			sprintf_s(msg, 409600, "Handle request :", request_data.c_str());
			BG_LOG_INFO(msg);

			// ����������ת��Ϊjson������д���
			// �������������Ƿ���������Ҫ���ƴ����쳣���������Ӧ��ʹ�÷���ʽ���
			Json::Reader reader;
			Json::Value json_root;

			if (!reader.parse(request_data, json_root))
				errCode = ERROR_BAD_COMMAND;
			else
			{
				// �����ؽ��
				Json::Value result;
				
				// ���ȼ�鼸�������е��ֶ�
				// id/commandtype/subcmd/manufacturer
				if (json_root.isMember("id") && json_root.isMember("commandtype") && json_root.isMember("subcmd") && json_root.isMember("manufacturer"))
				{
					result["id"] = Json::Value(json_root["id"].asString());

					if (json_root["commandtype"].asString().compare("PTZControl") == 0)
					{
						// ������̨����ָ��
						std::string subcmd = json_root["subcmd"].asString();
						std::string manufacturer = json_root["manufacturer"].asString();

						// ��̨������������еĲ�����state/value/speed
						if (json_root.isMember("state") && json_root.isMember("value") && json_root.isMember("speed"))
						{
							std::string cmdval = json_root["value"].asString();
							std::string cmdstate = json_root["state"].asString();
							int speed = json_root["speed"].asInt();

							// ���Ϳ���ָ��豸
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
						// ���ǵ���ָ����豸���͵�������
						std::string subcmd = json_root["subcmd"].asString();
						std::string manufacturer = json_root["manufacturer"].asString();

						
						if (_stricmp("realvideo", subcmd.c_str()) == 0)
						{
							// ʵʱ��������ж��ǿ�ʼ�������ǽ�������
							if (json_root.isMember("state"))
							{
								std::string cmdstate = json_root["state"].asString();
								if (_stricmp(cmdstate.c_str(), "Start") == 0)
								{
									// ��ʼ����ж����ĸ�
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

			// �������۴���ɹ�����ʧ�ܣ����Ƕ�Ӧ���ͷŵ����Ԫ��
			delete [] cmddata.cmddata_;
			cmddata.cmddata_ = nullptr;
			cmddata.cmddata_buffer_len_ = 0;
			cmddata.cmddata_size_ = 0;

			// ����Ĳ���Ӧ������Ҫ�����ģ��������Ȳ�����
			cmds_.erase(connect_id);
		}
	}
	else
		errCode = ERROR_NOT_SUPPORTED;

	// �������֮�󷵻ش�����

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