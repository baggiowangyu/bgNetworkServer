#ifndef _BG_DEVICE_CONTROL_H_
#define _BG_DEVICE_CONTROL_H_

#include "bgHttpBusinessPlugins.h"

#include <iostream>

class bgDeviceControl : public bgHttpBusinessPlugins
{
public:
	bgDeviceControl();
	~bgDeviceControl();

public:
	/**
	 * 判断是否为自己关心的消息，在HTTP头处理完毕之后就可以判断
	 * 如果是，并且有附带数据，则设置实体数据总长度
	 */
	virtual bool IsMyMsg(const char *path);

	/**
	 * 设置HTTP实体数据总长度，准备好相应的
	 */
	virtual int SetHttpContentLength(int data_len);

	/**
	 * 缓存HTTP实体数据
	 * 参数：
	 *	@data		实体数据
	 *	@data_len	数据长度
	 */ 
	virtual int CacheHttpContentData(const unsigned char *data, int data_len);

	/**
	 * 处理请求数据
	 */
	virtual int HandleRequest(const char *path, const char *query);

public:
	std::string cache_file_path_;
	FILE *cache_content_file_;

};


extern "C" 
{
	bgHttpBusinessPlugins* __stdcall CreateObject();
	void __stdcall DestroyObject(bgHttpBusinessPlugins* plugin);
};

#endif//_BG_DEVICE_CONTROL_H_