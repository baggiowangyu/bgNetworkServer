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
// 大华设备控制类
// 关于点流问题的描述：
// 设备控制端可以记录提交点流的connect_id
// 当它们与服务器断开连接的时候，清除对应的记录，当记录清零后，停止推流
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
	 * 设备登录
	 */
	int OnLogin(const char *device_ip, unsigned short device_port, const char *username, const char *password);

	/**
	 * 退出设备登录
	 */
	int OnLogout();

public:
	int OnPTZControl(const char *cmdtype, const char *cmd, int param1, int param2, int param3, BOOL stop = FALSE);

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
	bool IsRealPlay();

public:
	/**
	 * 实时流数据缓冲与读取
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
	unsigned char cache_data_[40960];	// 默认数据缓冲区40K
	int cache_data_len_;
};

#endif//_BG_DAHUA_DEVICE_CONTROL_H_
