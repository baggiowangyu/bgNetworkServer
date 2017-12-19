#include "bgDaHuaDeviceControl.h"
#include "dhnetsdk.h"

#include <iostream>

#define TEST_WRITEFILE



bgDahuaDeviceControl::bgDahuaDeviceControl(bgDahuaDeviceRealStreamNotifer *notifer /* = nullptr */)
	: notifer_(notifer)
	, move_speed_(4)
{
	CLIENT_Init(NULL, 0);
}

bgDahuaDeviceControl::~bgDahuaDeviceControl()
{
	CLIENT_Cleanup();
}

int bgDahuaDeviceControl::OnLogin(const char *device_ip, unsigned short device_port, const char *username, const char *password)
{
	int errCode = 0;

	login_id_ = CLIENT_LoginEx2(device_ip, device_port, username, password, EM_LOGIN_SPEC_CAP_TCP, nullptr, &device_info_, &errCode);
	if (login_id_ == 0)
	{
		// 登录失败
		return errCode;
	}

	// 登录成功后，查询设备状态
	// 我们在这里先什么都不干

	return errCode;
}

int bgDahuaDeviceControl::OnLogout()
{
	return CLIENT_Logout(login_id_);
}

int bgDahuaDeviceControl::OnPTZControl_TurnUp(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_UP_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnUpRight(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_EXTPTZ_RIGHTTOP, move_speed_, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnRight(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_RIGHT_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnDownRight(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_EXTPTZ_RIGHTDOWN, move_speed_, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnDown(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_DOWN_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnDownLeft(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_EXTPTZ_LEFTDOWN, move_speed_, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnLeft(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_LEFT_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnUpLeft(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_EXTPTZ_LEFTTOP, move_speed_, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_ZoomAdd(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_ZOOM_ADD_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_ZoomDec(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_ZOOM_DEC_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_FocusAdd(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_FOCUS_ADD_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_FocusDec(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_FOCUS_DEC_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_ApertureAdd(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_APERTURE_ADD_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_ApertureDec(BOOL stop /* = FALSE */)
{
	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_APERTURE_DEC_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnStartRealPlay()
{
	int errCode = 0;

	real_handle_ = CLIENT_StartRealPlay(login_id_, 0, nullptr, DH_RType_Realplay, bgDahuaDeviceControl::_fRealDataCallBackEx, bgDahuaDeviceControl::_fRealPlayDisConnect, (LDWORD)this);

	return errCode;
}

int bgDahuaDeviceControl::OnStopRealPlay()
{
	int errCode = 0;

	CLIENT_StopRealPlayEx(real_handle_);

	return errCode;
}

void bgDahuaDeviceControl::_fRealDataCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, LDWORD dwUser)
{
	bgDahuaDeviceControl *device_control = (bgDahuaDeviceControl *)dwUser;
	// 这里用于接收流数据，组织推送到EasyDarwin上
	std::string media_type;
	switch (dwDataType)
	{
	case 0:
		media_type = "原始数据";
		break;
	case 1:
		media_type = "帧数据";
		break;
	case 2:
		media_type = "yuv数据";
		break;
	case 3:
		media_type = "pcm音频数据";
		break;
	}

	std::cout<<"收到媒体数据类型："<<media_type.c_str()<<std::endl;

	if (device_control->notifer_)
		device_control->notifer_->RealStreamDataNotify(dwDataType, pBuffer, dwBufSize, param, dwUser);

	// 实际数据是否需要写入到文件？
#ifdef TEST_WRITEFILE
	FILE *record_file = fopen("record.r", "ab+");
	if (record_file)
	{
		fwrite(pBuffer, dwBufSize, 1, record_file);
		fflush(record_file);
		fclose(record_file);
		record_file = nullptr;
	}
#endif
}

void bgDahuaDeviceControl::_fRealPlayDisConnect(LLONG lOperateHandle, EM_REALPLAY_DISCONNECT_EVENT_TYPE dwEventType, void* param, LDWORD dwUser)
{
	// 断开通知，应该什么都不用做
}