#ifndef _BG_DAHUA_DEVICE_CONTROL_H_
#define _BG_DAHUA_DEVICE_CONTROL_H_

#include <windows.h>


class bgDahuaDeviceRealStreamNotifer
{
public:
	virtual void RealStreamDataNotify(DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, DWORD dwUser) = 0;
};

//////////////////////////////////////////////////////////////////////////
//
// ���豸������
// ���ڵ��������������
// �豸���ƶ˿��Լ�¼�ύ������connect_id
// ��������������Ͽ����ӵ�ʱ�������Ӧ�ļ�¼������¼�����ֹͣ����
//
//////////////////////////////////////////////////////////////////////////

class bgDahuaDeviceControl
{
public:
	bgDahuaDeviceControl(bgDahuaDeviceRealStreamNotifer *notifer = nullptr);
	~bgDahuaDeviceControl();

public:
	int OnInit(const char *config_ini);

	/**
	 * �豸��¼
	 */
	int OnLogin(const char *device_ip, unsigned short device_port, const char *username, const char *password);

	/**
	 * �˳��豸��¼
	 */
	int OnLogout();

public:
	int OnPTZControl(const char *cmdtype, const char *cmd, int param1, int param2, int param3, BOOL stop = FALSE);

public:
	/**
	 * ��̨���ƣ�����ת��
	 */
	int OnPTZControl_TurnUp(BOOL stop = FALSE);
	int OnPTZControl_TurnUpRight(BOOL stop = FALSE);
	int OnPTZControl_TurnRight(BOOL stop = FALSE);
	int OnPTZControl_TurnDownRight(BOOL stop = FALSE);
	int OnPTZControl_TurnDown(BOOL stop = FALSE);
	int OnPTZControl_TurnDownLeft(BOOL stop = FALSE);
	int OnPTZControl_TurnLeft(BOOL stop = FALSE);
	int OnPTZControl_TurnUpLeft(BOOL stop = FALSE);

	/**
	 * ��̨���ƣ����������ࡢ��Ȧ
	 */
	int OnPTZControl_ZoomAdd(BOOL stop = FALSE);
	int OnPTZControl_ZoomDec(BOOL stop = FALSE);
	int OnPTZControl_FocusAdd(BOOL stop = FALSE);
	int OnPTZControl_FocusDec(BOOL stop = FALSE);
	int OnPTZControl_ApertureAdd(BOOL stop = FALSE);
	int OnPTZControl_ApertureDec(BOOL stop = FALSE);

public:
	/**
	 * ��Ƶ�㲥
	 */
	int OnStartRealPlay();
	int OnStopRealPlay();
	bool IsRealPlay();

public:
	/**
	 * ʵʱ�����ݻ������ȡ
	 */
	void CacheRealStreamData(const unsigned char *data, int data_len);
	void ReadCacheRealStreamData(unsigned char **data, int *data_len);
	void ReleaseCacheResource(unsigned char **data);

public:
	bgDahuaDeviceRealStreamNotifer *notifer_;

private:
	LONG login_id_;
	LONG real_handle_;
	int move_speed_;

private:
	CRITICAL_SECTION cache_section_;
	unsigned char cache_data_[40960];	// Ĭ�����ݻ�����40K
	int cache_data_len_;
};

#endif//_BG_DAHUA_DEVICE_CONTROL_H_
