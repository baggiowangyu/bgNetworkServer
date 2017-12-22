#include "StreamPusherManagement.h"

DWORD WINAPI WorkingThread(LPVOID lpParam)
{
	StreamPusherManagement *pusher = (StreamPusherManagement*)lpParam;
	pusher->is_working_ = true;

	while (true)
	{
		if (!pusher->is_working_)
			break;

		DWORD ret = WaitForSingleObject(pusher->shell_info_.hProcess, 1000);
		if (ret == WAIT_FAILED)
		{
			// �ȴ�ʧ�ܣ������ǽ����˳���
			// ���µ��������ӿڣ�Ȼ��ǰ�����߳��˳�
			pusher->StartPush(pusher->source_url_.c_str(), pusher->target_url_.c_str());
			break;
		}
		else if (ret == WAIT_OBJECT_0)
		{
			// �����˳���
			// ���µ��������ӿڣ�Ȼ��ǰ�����߳��˳�
			pusher->StartPush(pusher->source_url_.c_str(), pusher->target_url_.c_str());
			break;
		}
		else if (WAIT_ABANDONED == ret)
		{
			// ��֪���Ǹ�ɶԭ��
			// ���µ��������ӿڣ�Ȼ��ǰ�����߳��˳�
			pusher->StartPush(pusher->source_url_.c_str(), pusher->target_url_.c_str());
			break;
		}
		else if (WAIT_TIMEOUT == ret)
		{
			// ���̻��ڣ������ǸϽ�������һ��ѭ��
			continue;
		}
		else
		{
			// ��������
			// ���µ��������ӿڣ�Ȼ��ǰ�����߳��˳�
			pusher->StartPush(pusher->source_url_.c_str(), pusher->target_url_.c_str());
			break;
		}
	}

	return 0;
}

StreamPusherManagement::StreamPusherManagement()
	: working_(nullptr)
	, keep_push_(true)
	, is_working_(false)
{

}

StreamPusherManagement::~StreamPusherManagement()
{

}

int StreamPusherManagement::StartPush(const char *source_url, const char *target_ip, const char *target_port /* = "554" */, const char *protocol /* = "rtsp" */, const char *stream_name /* = "car_video.sdp" */, bool keep_push /* = true */)
{
	int errCode = 0;

	source_url_ = source_url;
	target_url_ = target_ip;
	keep_push_ = keep_push;

	char param[4096] = {0};
	if (_stricmp("rtsp", protocol) == 0)
		sprintf_s(param, 4096, "-i %s -vcodec copy -acodec copy  -rtsp_transport tcp -f rtsp %s://%s:%s/%s", source_url, protocol, target_ip, target_port, stream_name);
	else
		return ERROR_NOT_SUPPORTED;

	// ������������������Ȼ���߳�����������
	// ��������˳�������
	
	memset(&shell_info_, 0, sizeof(SHELLEXECUTEINFOA));
	shell_info_.cbSize = sizeof(SHELLEXECUTEINFOA);
	shell_info_.fMask = SEE_MASK_NOCLOSEPROCESS;
	shell_info_.lpFile = "ffmpeg.exe";
	shell_info_.lpParameters = param;
	shell_info_.lpVerb = "open";
	shell_info_.nShow = SW_SHOW;

	BOOL bret = ShellExecuteExA(&shell_info_);
	if (!bret)
	{
		errCode = GetLastError();
		return errCode;
	}

	working_ = CreateThread(NULL, 0, WorkingThread, this, 0, NULL);
	if (!working_)
	{
		errCode = GetLastError();
		return errCode;
	}

	return 0;
}

void StreamPusherManagement::StopPush()
{
	is_working_ = false;

	// ��������
	TerminateProcess(shell_info_.hProcess, 4);
}