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
	 * 设备登录
	 */
	int OnLogin(const char *device_ip, unsigned short device_port, const char *username, const char *password);

	/**
	 * 退出设备登录
	 */
	int OnLogout();

public:
	/**
	 * 云台控制，方向转动
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
	 * 云台控制，倍数、焦距、光圈
	 */
	int OnPTZControl_ZoomAdd(BOOL stop = FALSE);
	int OnPTZControl_ZoomDec(BOOL stop = FALSE);
	int OnPTZControl_FocusAdd(BOOL stop = FALSE);
	int OnPTZControl_FocusDec(BOOL stop = FALSE);
	int OnPTZControl_ApertureAdd(BOOL stop = FALSE);
	int OnPTZControl_ApertureDec(BOOL stop = FALSE);

public:
	/**
	 * 视频点播
	 */
	int OnStartRealPlay();
	int OnStopRealPlay();

public:
	/**
	 * 视频监视数据回调函数
	 */
	static void CALLBACK _fRealDataCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, LDWORD dwUser);

	/**
	 * 视频监视断开回调
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
