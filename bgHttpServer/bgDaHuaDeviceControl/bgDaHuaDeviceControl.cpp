#include "bgDaHuaDeviceControl.h"
#include "StreamPusherManagement.h"
#include "dhnetsdk.h"
#include <iostream>

#define CFG_NAME				"DH_DEV"
#define STREAM_SERVER_NAME		"STREAM_SERVER"

#define TEST_WRITEFILE

//////////////////////////////////////////////////////////////////////////
//
// �豸��¼���߻ص�
//
//////////////////////////////////////////////////////////////////////////

void CALLBACK _fDisConnect(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
	bgDahuaDeviceControl *device_control = (bgDahuaDeviceControl*)dwUser;

	// �Ͽ�����
	device_control->OnStopRealPlay();

	// ����״̬����Ϊ�Ͽ�����
	device_control->is_device_connected_ = false;

	// �������µ�¼��ֱ����¼�ɹ�
	while (true)
	{
		int errCode = device_control->OnInit(device_control->config_path_.c_str());
		if (errCode == 0)
			break;
	}
	
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

void CALLBACK _fRealDataCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, LDWORD dwUser)
{
	bgDahuaDeviceControl *device_control = (bgDahuaDeviceControl *)dwUser;


	// �������ڽ��������ݣ���֯���͵�EasyDarwin��
	std::string media_type;
	switch (dwDataType)
	{
	case 0:
		media_type = "ԭʼ����";
		break;
	case 1:
		media_type = "֡����";
		break;
	case 2:
		media_type = "yuv����";
		break;
	case 3:
		media_type = "pcm��Ƶ����";
		break;
	}

	std::cout<<"�յ�ý���������ͣ�"<<media_type.c_str()<<" ���ݳ��ȣ�"<<dwBufSize<<std::endl;

	if (device_control->notifer_)
		device_control->notifer_->RealStreamDataNotify(dwDataType, pBuffer, dwBufSize, param, dwUser);

	// ���»�����
	device_control->CacheRealStreamData(pBuffer, dwBufSize);

	// ʵ�������Ƿ���Ҫд�뵽�ļ���
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

	// ����ʵ�ʵ���Ƶ���ļ�������������ݲ������£�
	// ��Ƶ���룺H.264
	// ��Ƶ�ߴ磺1920 * 1080
	// Դ֡�ʣ�25fps
	// ɫ��ģʽ��YUV420
}

void CALLBACK _fRealPlayDisConnect(LLONG lOperateHandle, EM_REALPLAY_DISCONNECT_EVENT_TYPE dwEventType, void* param, LDWORD dwUser)
{
	// �Ͽ�֪ͨ��Ӧ��ʲô��������
}


bgDahuaDeviceControl::bgDahuaDeviceControl(bgDahuaDeviceRealStreamNotifer *notifer /* = nullptr */)
	: notifer_(notifer)
	, move_speed_(4)
	, cache_data_len_(0)
	, stream_pusher_management_(new StreamPusherManagement())
	, is_device_connected_(false)
{
	// ����ʵ���ϻ�Ҫ���豸���߻ص�
	// �ص�����Ҫ�����������µ�¼�Ĺ���
	CLIENT_Init(_fDisConnect, (DWORD)this);
}

bgDahuaDeviceControl::~bgDahuaDeviceControl()
{
	delete stream_pusher_management_;
	CLIENT_Cleanup();
}

int bgDahuaDeviceControl::OnInit(const char *config_ini)
{
	// ��ȡ�����ļ�
	config_path_ = config_ini;

	char ip[4096] = {0};
	unsigned short port = 0;
	char username[4096] = {0};
	char password[4096] = {0};
	char stream_server_protocol[4096] = {0};
	char stream_server_ip[4096] = {0};
	char stream_server_port[4096] = {0};
	char source_stream_url[4096] = {0};
	char target_stream_name[4096] = {0};
	char target_stream_url[4096] = {0};

	GetPrivateProfileStringA(CFG_NAME, "DEV_IP", "192.168.1.108", ip, 4096, config_ini);
	port = (unsigned short)GetPrivateProfileIntA(CFG_NAME, "DEV_PORT", 37777, config_ini);
	GetPrivateProfileStringA(CFG_NAME, "DEV_USER", "admin", username, 4096, config_ini);
	GetPrivateProfileStringA(CFG_NAME, "DEV_PASS", "admin", password, 4096, config_ini);
	int auto_realstream = GetPrivateProfileIntA(CFG_NAME, "AUTO_REALSTREAM", 1, config_ini);
	GetPrivateProfileStringA(CFG_NAME, "SOURCE_STREAM", "rtsp://admin:admin@192.168.1.108/", source_stream_url, 4096, config_ini);
	GetPrivateProfileStringA(CFG_NAME, "TARGET_STREAM_NAME", "car_video.sdp", target_stream_name, 4096, config_ini);
	GetPrivateProfileStringA(CFG_NAME, "TARGET_STREAM_URL", "rtsp://127.0.0.1/car_video.sdp", target_stream_url, 4096, config_ini);

	GetPrivateProfileStringA(STREAM_SERVER_NAME, "PROTOCOL", "RTSP", stream_server_protocol, 4096, config_ini);
	GetPrivateProfileStringA(STREAM_SERVER_NAME, "IP", "127.0.0.1", stream_server_ip, 4096, config_ini);
	GetPrivateProfileStringA(STREAM_SERVER_NAME, "PORT", "554", stream_server_port, 4096, config_ini);

	device_ip_ = ip;
	device_username_ = username;
	device_password_ = password;

	stream_server_protocol_ = stream_server_protocol;
	stream_server_ip_ = stream_server_ip;
	stream_server_port_ = stream_server_port;
	source_stream_url_ = source_stream_url;
	target_stream_name_ = target_stream_name;
	target_stream_url_ = target_stream_url;

	// ��¼
	int errCode = 0;
	while (true)
	{
		errCode = OnLogin(ip, port, username, password);
		if (errCode != 0)
		{
			OnStopRealPlay();
			Sleep(100);
			continue;
		}

		break;
	}
	
	// ��������
	if (auto_realstream)
		errCode = OnStartRealPlay();

	return errCode;
}

int bgDahuaDeviceControl::OnLogin(const char *device_ip, unsigned short device_port, const char *username, const char *password)
{
	int errCode = 0;

	device_ip_ = device_ip;
	device_port_ = device_port;

	NET_DEVICEINFO_Ex device_info_;
	login_id_ = CLIENT_LoginEx2(device_ip, device_port, username, password, EM_LOGIN_SPEC_CAP_TCP, nullptr, &device_info_, &errCode);
	if (login_id_ == 0)
	{
		// ��¼ʧ��
		std::cout<<"DH_DeviceControl login failed... errCode : "<<errCode<<std::endl;
		return -1;
	}

	is_device_connected_ = true;

	// ��¼�ɹ��󣬲�ѯ�豸״̬
	// ���ǻ�������һЩ���飺
	//  - Уʱ
	//  - ����ˮӡ
	//  - ...
	// ����������������ʲô������

	return 0;
}

int bgDahuaDeviceControl::OnLogout()
{
	return CLIENT_Logout(login_id_);
}

int bgDahuaDeviceControl::OnPTZControl(const char *cmdtype, const char *cmd, int param1, int param2, int param3, BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	// �������˶��ٶ�
	move_speed_ = param2;

	if (_stricmp(cmdtype, "direction") == 0)
	{
		// �����������
		if (_stricmp(cmd, "Up") == 0)
			return OnPTZControl_TurnUp(stop);
		else if (_stricmp(cmd, "UpRight") == 0)
			return OnPTZControl_TurnUpRight(stop);
		else if (_stricmp(cmd, "Right") == 0)
			return OnPTZControl_TurnRight(stop);
		else if (_stricmp(cmd, "DownRight") == 0)
			return OnPTZControl_TurnDownRight(stop);
		else if (_stricmp(cmd, "Down") == 0)
			return OnPTZControl_TurnDown(stop);
		else if (_stricmp(cmd, "DownLeft") == 0)
			return OnPTZControl_TurnDownLeft(stop);
		else if (_stricmp(cmd, "Left") == 0)
			return OnPTZControl_TurnLeft(stop);
		else if (_stricmp(cmd, "UpLeft") == 0)
			return OnPTZControl_TurnUpLeft(stop);
		else
			return ERROR_NOT_SUPPORTED;
	}
	else if (_stricmp(cmdtype, "camera") == 0)
	{
		// ��ͷ��������
		if (_stricmp(cmd, "Plus") == 0)
			return OnPTZControl_ZoomAdd(stop);
		else if (_stricmp(cmd, "Minus") == 0)
			return OnPTZControl_ZoomDec(stop);
		else
			return ERROR_NOT_SUPPORTED;
	}
	else if (_stricmp(cmdtype, "focus") == 0)
	{
		// �����������
		if (_stricmp(cmd, "Plus") == 0)
			return OnPTZControl_FocusAdd(stop);
		else if (_stricmp(cmd, "Minus") == 0)
			return OnPTZControl_FocusDec(stop);
		else
			return ERROR_NOT_SUPPORTED;
	}
	else if (_stricmp(cmdtype, "aperture") == 0)
	{
		// ��Ȧ��������
		if (_stricmp(cmd, "Plus") == 0)
			return OnPTZControl_ApertureAdd(stop);
		else if (_stricmp(cmd, "Minus") == 0)
			return OnPTZControl_ApertureDec(stop);
		else
			return ERROR_NOT_SUPPORTED;
	}

	return ERROR_NOT_SUPPORTED;
}

int bgDahuaDeviceControl::OnPTZControl_TurnUp(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_UP_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnUpRight(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_EXTPTZ_RIGHTTOP, move_speed_, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnRight(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_RIGHT_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnDownRight(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_EXTPTZ_RIGHTDOWN, move_speed_, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnDown(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_DOWN_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnDownLeft(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_EXTPTZ_LEFTDOWN, move_speed_, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnLeft(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_LEFT_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_TurnUpLeft(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_EXTPTZ_LEFTTOP, move_speed_, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_ZoomAdd(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_ZOOM_ADD_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_ZoomDec(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_ZOOM_DEC_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_FocusAdd(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_FOCUS_ADD_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_FocusDec(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_FOCUS_DEC_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_ApertureAdd(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_APERTURE_ADD_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnPTZControl_ApertureDec(BOOL stop /* = FALSE */)
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	BOOL bret = CLIENT_DHPTZControl(login_id_, 0, DH_PTZ_APERTURE_DEC_CONTROL, 0, move_speed_, 0, stop);

	if (!bret)
		return -1;
	else
		return 0;
}

int bgDahuaDeviceControl::OnStartRealPlay()
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	int errCode = 0;

	//real_handle_ = CLIENT_StartRealPlay(login_id_, 0, nullptr, DH_RType_Realplay, _fRealDataCallBackEx, _fRealPlayDisConnect, (LDWORD)this);
	//if (real_handle_ == 0)
	//	return -1;

	real_handle_ = CLIENT_RealPlayEx(login_id_, 0, nullptr);
	if (real_handle_ == 0)
		return -1;

	//// ��װ���ݶ�ȡ�ص���Ҫ��������Ǳ�׼��MPEG/H264��׼����
	//// ��������Լ�д���������Ͱ�װ����ص�������������ʱ����Ҫ
	// BOOL bret = CLIENT_SetRealDataCallBackEx(real_handle_, _fRealDataCallBackEx, (DWORD)this, 0x0000001f);

	// �ɹ�֮�󣬽���Ϣ������������׼������
	// ����һ���������У�ֱ����ffmpeg����ת�������ڳ��ض�
	// ffmpeg.exe -i rtsp://admin:admin@192.168.1.108/cam/realmonitor?channel=1&subtype=0 -vcodec copy -acodec copy  -rtsp_transport tcp -f rtsp rtsp://127.0.0.1:554/dh_dvr.sdp
	// ������ͷ��rtsp������ʽ��rtsp://[username]:[password]@[ip]:[port]/cam/realmonitor?channel=1&subtype=0
	// ��������ͷ��rtsp������ʽ��rtsp://[username]:[password]@[ip]:[port]/[codec]/[channel]/[subtype]/av_stream
	// ������
	// 1. ffmpeg.exe������·��
	// 2. ����ͷ��¼�˺š�����
	// 3. ����ͷIP��ַ
	// 4. ����Ŀ���������ַ

	// modify by wangyu - 20171229
	// �·������������ǲ����������ò�����ֱ�ӵ�EasyDarwin����ȡRTSP��
	//if (!stream_pusher_management_->is_working_)
	//	errCode = stream_pusher_management_->StartPush(source_stream_url_.c_str(), stream_server_ip_.c_str(), stream_server_port_.c_str(), stream_server_protocol_.c_str(), target_stream_name_.c_str());

	return errCode;
}

int bgDahuaDeviceControl::OnStopRealPlay()
{
	if (!is_device_connected_)
		return ERROR_NOT_READY;

	int errCode = 0;

	stream_pusher_management_->is_working_ = false;
	stream_pusher_management_->StopPush();
	CLIENT_StopRealPlayEx(real_handle_);

	return errCode;
}

bool bgDahuaDeviceControl::IsRealPlay()
{
	return stream_pusher_management_->is_working_;
}

std::string bgDahuaDeviceControl::GetRealStreamProtocol()
{
	return stream_server_protocol_;
}

std::string bgDahuaDeviceControl::GetRealStreamName()
{
	return target_stream_name_;
}

std::string bgDahuaDeviceControl::GetRealStreamUrl()
{
	return target_stream_url_;
}

void bgDahuaDeviceControl::CacheRealStreamData(const unsigned char *data, int data_len)
{
	EnterCriticalSection(&cache_section_);

	memcpy(cache_data_, data, data_len);
	cache_data_len_ = data_len;

	LeaveCriticalSection(&cache_section_);
}

void bgDahuaDeviceControl::ReadCacheRealStreamData(unsigned char **data, int *data_len)
{
	EnterCriticalSection(&cache_section_);

	*data = new unsigned char[cache_data_len_];
	memcpy(*data, cache_data_, cache_data_len_);
	*data_len = cache_data_len_;

	LeaveCriticalSection(&cache_section_);
}

void bgDahuaDeviceControl::ReleaseCacheResource(unsigned char **data)
{
	delete [] *data;
	*data = nullptr;
}