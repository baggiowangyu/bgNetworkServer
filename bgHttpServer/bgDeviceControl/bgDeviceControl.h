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
	 * 初始化配置信息
	 */
	virtual int Init(const char *config_ini);

	/**
	 * 判断是否为自己关心的消息，在HTTP头处理完毕之后就可以判断
	 * 如果是，并且有附带数据，则设置实体数据总长度
	 */
	virtual bool IsMyMsg(unsigned long connect_id, const char *method, const char *path);

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
	virtual int HandleRequest(unsigned long connect_id, const char *method, const char *path, unsigned char **response_data, int *response_len, const char *query = nullptr);

public:
	std::string cache_file_path_;
	FILE *cache_content_file_;

	// 这里要考虑到多个客户端连进来的情况，需要缓存对应的POST数据

};


extern "C" 
{
	bgHttpBusinessPlugins* __stdcall CreateObject();
	void __stdcall DestroyObject(bgHttpBusinessPlugins* plugin);
};

#endif//_BG_DEVICE_CONTROL_H_