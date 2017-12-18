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
	 * ��ʼ��������Ϣ
	 */
	virtual int Init(const char *config_ini);

	/**
	 * �ж��Ƿ�Ϊ�Լ����ĵ���Ϣ����HTTPͷ�������֮��Ϳ����ж�
	 * ����ǣ������и������ݣ�������ʵ�������ܳ���
	 */
	virtual bool IsMyMsg(unsigned long connect_id, const char *method, const char *path);

	/**
	 * ����HTTPʵ�������ܳ��ȣ�׼������Ӧ��
	 */
	virtual int SetHttpContentLength(int data_len);

	/**
	 * ����HTTPʵ������
	 * ������
	 *	@data		ʵ������
	 *	@data_len	���ݳ���
	 */ 
	virtual int CacheHttpContentData(const unsigned char *data, int data_len);

	/**
	 * ������������
	 */
	virtual int HandleRequest(unsigned long connect_id, const char *method, const char *path, unsigned char **response_data, int *response_len, const char *query = nullptr);

public:
	std::string cache_file_path_;
	FILE *cache_content_file_;

	// ����Ҫ���ǵ�����ͻ������������������Ҫ�����Ӧ��POST����

};


extern "C" 
{
	bgHttpBusinessPlugins* __stdcall CreateObject();
	void __stdcall DestroyObject(bgHttpBusinessPlugins* plugin);
};

#endif//_BG_DEVICE_CONTROL_H_