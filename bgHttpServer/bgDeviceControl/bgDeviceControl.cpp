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

bool bgDeviceControl::IsMyMsg(const char *path)
{
	if (_stricmp(path, CURRENT_MODULE_PATH) != 0)
		return false;
	else
		return true;
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

int bgDeviceControl::HandleRequest(const char *path, const char *query)
{
	int errCode = 0;

	// ��ȡ�ļ�����������
	// �ļ�����Ӧ����һ��json

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