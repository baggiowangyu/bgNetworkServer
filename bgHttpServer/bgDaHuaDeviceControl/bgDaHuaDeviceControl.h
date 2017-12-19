#ifndef _BG_DAHUA_DEVICE_CONTROL_H_
#define _BG_DAHUA_DEVICE_CONTROL_H_

#include "dhnetsdk.h"

class bgDahuaDeviceRealStreamNotifer
{
public:
	virtual void RealStreamDataNotify(DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, LDWORD dwUser) = 0;
};

class bgDahuaDeviceControl
{
public:
	bgDahuaDeviceControl(bgDahuaDeviceRealStreamNotifer *notifer = nullptr);
	~bgDahuaDeviceControl();

public:
	/**
	 * �豸��¼
	 */
	int OnLogin(const char *device_ip, unsigned short device_port, const char *username, const char *password);

	/**
	 * �˳��豸��¼
	 */
	int OnLogout();

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

public:
	/**
	 * ��Ƶ�������ݻص�����
	 */
	static void CALLBACK _fRealDataCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, LDWORD dwUser);

	/**
	 * ��Ƶ���ӶϿ��ص�
	 */
	static void CALLBACK _fRealPlayDisConnect(LLONG lOperateHandle, EM_REALPLAY_DISCONNECT_EVENT_TYPE dwEventType, void* param, LDWORD dwUser);

public:
	bgDahuaDeviceRealStreamNotifer *notifer_;

private:
	NET_DEVICEINFO_Ex device_info_;
	LLONG login_id_;
	LLONG real_handle_;

	int move_speed_;
};

#endif//_BG_DAHUA_DEVICE_CONTROL_H_
