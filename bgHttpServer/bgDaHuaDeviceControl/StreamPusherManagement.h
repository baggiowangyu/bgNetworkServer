#ifndef _STREAM_PUSHER_MANAGEMENT_H_
#define _STREAM_PUSHER_MANAGEMENT_H_

#include <string>
#include <Windows.h>
#include <Shellapi.h>

//////////////////////////////////////////////////////////////////////////
//
// 推流器管理模块
// 设定好源地址和目标地址，从源地址拉流后推到目标地址
// 可持续推流
//
//////////////////////////////////////////////////////////////////////////

class StreamPusherManagement
{
public:
	StreamPusherManagement();
	~StreamPusherManagement();

public:
	int StartPush(const char *source_url, const char *target_ip, const char *target_port = "554", const char *protocol = "rtsp", const char *stream_name = "car_video.sdp", bool keep_push = true);
	void StopPush();

private:
	bool IsExistPusher(const char *commandline);

public:
	SHELLEXECUTEINFOA shell_info_;
	HANDLE pusher_handle_;
	std::string source_url_;
	std::string target_url_;
	bool keep_push_;
	bool is_working_;
	bool is_have_thread_;

private:
	HANDLE working_;
};

#endif//_STREAM_PUSHER_MANAGEMENT_H_