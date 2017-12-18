// bgDeviceControl.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "bgDeviceControl.h"

#define CURRENT_MODULE_PATH "/goldmsg/DeviceControl"

bgDeviceControl::bgDeviceControl()
	: cache_content_file_(nullptr)
{

}

bgDeviceControl::~bgDeviceControl()
{

}

int bgDeviceControl::Init(const char *config_ini)
{
	int errCode = 0;

	return errCode;
}

bool bgDeviceControl::IsMyMsg(unsigned long connect_id, const char *method, const char *path)
{
	if (_stricmp(path, CURRENT_MODULE_PATH) != 0)
		return false;
	else
	{
		if (_stricmp(method, "GET") == 0)
			return true;
		else if (_stricmp(method, "POST") == 0)
		{
			// ����һ�����ļ���������д
			return true;
		}
		else
		{
			return false;
		}
	}
		
}

int bgDeviceControl::SetHttpContentLength(int data_len)
{
	int errCode = 0;

	// ÿ�ν����������˵����ǰ���µĲ��������ˣ�����һ����ʱ�ļ�
	SYSTEMTIME st;
	GetLocalTime(&st);

	char tmp_path[4096] = {0};
	GetTempFileNameA("BG_", "TEMP_", 4096, tmp_path);

	cache_file_path_ = tmp_path;

	cache_content_file_ = fopen(tmp_path, "rwb");
	if (cache_content_file_ == nullptr)
		return -1;

	return errCode;
}

int bgDeviceControl::CacheHttpContentData(const unsigned char *data, int data_len)
{
	int errCode = 0;

	if (cache_content_file_)
		fwrite(data, 1, data_len, cache_content_file_);

	return errCode;
}

int bgDeviceControl::HandleRequest(unsigned long connect_id, const char *method, const char *path, unsigned char **response_data, int *response_len, const char *query /* = nullptr */)
{
	int errCode = 0;

	if (_stricmp("GET", method) == 0)
	{
		if (!IsMyMsg(connect_id, method, path))
			errCode = ERROR_NOT_SUPPORTED;
		else
		{

		}

	}
	else if (_stricmp("POST", method) == 0)
	{
		if (!IsMyMsg(connect_id, method, path))
			errCode = ERROR_NOT_SUPPORTED;
		else
		{

		}
	}
	else
	{
		errCode = ERROR_NOT_SUPPORTED;
	}

	// �������֮�󷵻ش�����

	return errCode;
}

extern "C" bgHttpBusinessPlugins* __stdcall CreateObject()
{
	return (bgHttpBusinessPlugins*)new bgDeviceControl();
}

extern "C" void __stdcall DestroyObject(bgHttpBusinessPlugins* plugin)
{
	delete plugin;
	plugin = NULL;
}