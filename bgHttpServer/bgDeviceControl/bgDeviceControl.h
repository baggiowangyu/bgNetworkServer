#ifndef _BG_DEVICE_CONTROL_H_
#define _BG_DEVICE_CONTROL_H_

#include "bgHttpBusinessPlugins.h"

#include <iostream>
#include <map>

class bgDahuaDeviceControl;

typedef struct _COMMAND_DATA_
{
	unsigned char *cmddata_;
	unsigned long long cmddata_buffer_len_;
	unsigned long long cmddata_size_;
} COMMAND_DATA, *PCOMMAND_DATA;

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
	virtual int SetHttpContentLength(unsigned long connect_id, unsigned long long data_len);

	/**
	 * ����HTTPʵ������
	 * ������
	 *	@data		ʵ������
	 *	@data_len	���ݳ���
	 */ 
	virtual int CacheHttpContentData(unsigned long connect_id, const unsigned char *data, int data_len);

	/**
	 * ������������
	 */
	virtual int HandleRequest(unsigned long connect_id, const char *method, const char *path, unsigned char **response_data, int *response_len, const char *query = nullptr);

	virtual void CleanupResponseData(unsigned long connect_id, const char *method, unsigned char **response_data);

public:
	// ����Ҫ���ǵ�����ͻ������������������Ҫ�����Ӧ��POST����
	std::map<unsigned long, COMMAND_DATA> cmds_;
	bgDahuaDeviceControl *dahua_device_;
};


extern "C" 
{
	bgHttpBusinessPlugins* __stdcall CreateObject();
	void __stdcall DestroyObject(bgHttpBusinessPlugins** plugin);
};

#endif//_BG_DEVICE_CONTROL_H_