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
	int StartPush(const char *source_url, const char *target_url, const char *protocol = "rtsp", bool keep_push = true);
	void StopPush();

public:
	SHELLEXECUTEINFOA shell_info_;
	std::string source_url_;
	std::string target_url_;
	bool keep_push_;
	bool is_working_;

private:
	HANDLE working_;
};

#endif//_STREAM_PUSHER_MANAGEMENT_H_