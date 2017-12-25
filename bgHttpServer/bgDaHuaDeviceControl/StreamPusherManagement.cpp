#include "StreamPusherManagement.h"
#include "winternl.h"


typedef NTSTATUS  (WINAPI *NtQueryInformationProcessFake)(HANDLE, DWORD, PVOID, ULONG, PULONG);  
NtQueryInformationProcessFake ntQ = NULL;  


void getProcCMD(DWORD pid) {  
	HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid );  
	if (INVALID_HANDLE_VALUE != hproc){  
		HANDLE hnewdup = NULL;  
		PEB peb;  
		RTL_USER_PROCESS_PARAMETERS upps;  
		WCHAR buffer[MAX_PATH] = {NULL};  
		HMODULE hm = LoadLibrary(_T("Ntdll.dll"));  
		ntQ = (NtQueryInformationProcessFake)GetProcAddress(hm, "NtQueryInformationProcess");  
		if ( DuplicateHandle(GetCurrentProcess(), hproc, GetCurrentProcess(), &hnewdup, 0, FALSE, DUPLICATE_SAME_ACCESS) ) {  
			PROCESS_BASIC_INFORMATION pbi;  
			NTSTATUS isok = ntQ(hnewdup, 0/*ProcessBasicInformation*/, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), 0);          
			if (BCRYPT_SUCCESS(isok)) {  
				if ( ReadProcessMemory(hnewdup, pbi.PebBaseAddress, &peb, sizeof(PEB), 0) )  
					if ( ReadProcessMemory(hnewdup, peb.ProcessParameters, &upps, sizeof(RTL_USER_PROCESS_PARAMETERS), 0) ) {  
						WCHAR *buffer = new WCHAR[upps.CommandLine.Length + 1];  
						ZeroMemory(buffer, (upps.CommandLine.Length + 1) * sizeof(WCHAR));  
						ReadProcessMemory(hnewdup, upps.CommandLine.Buffer, buffer, upps.CommandLine.Length, 0);  
						delete buffer;  
					}  
			}  
			CloseHandle(hnewdup);  
		}  

		CloseHandle(hproc);  
	}
}

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
			// 等待失败，可能是进程退出了
			// 重新调用推流接口，然后当前工作线程退出
			pusher->StartPush(pusher->source_url_.c_str(), pusher->target_url_.c_str());
			break;
		}
		else if (ret == WAIT_OBJECT_0)
		{
			// 进程退出了
			// 重新调用推流接口，然后当前工作线程退出
			pusher->StartPush(pusher->source_url_.c_str(), pusher->target_url_.c_str());
			break;
		}
		else if (WAIT_ABANDONED == ret)
		{
			// 不知道是个啥原因
			// 重新调用推流接口，然后当前工作线程退出
			pusher->StartPush(pusher->source_url_.c_str(), pusher->target_url_.c_str());
			break;
		}
		else if (WAIT_TIMEOUT == ret)
		{
			// 进程还在，那我们赶紧进入下一个循环
			continue;
		}
		else
		{
			// 其他错误
			// 重新调用推流接口，然后当前工作线程退出
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

	// 检查当前是否存在一个ffmpeg正在执行推流
	// 找到该进程，查询命令行，如果命令行参数与我们要执行的一致，那么我们只要守护住对应的进程句柄即可
	// 如果没有找到一致的，那么我们就创建一个新的推流进程
	if (IsExistPusher(param))
	{
		return 0;
	}
	else
	{
		// 在这里启动推流器，然后开线程来守推流器
		// 如果开启了持续推流
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

		pusher_handle_ = shell_info_.hProcess;
		working_ = CreateThread(NULL, 0, WorkingThread, this, 0, NULL);
		if (!working_)
		{
			errCode = GetLastError();
			return errCode;
		}
	}

	return 0;
}

void StreamPusherManagement::StopPush()
{
	is_working_ = false;

	// 结束进程
	TerminateProcess(shell_info_.hProcess, 4);
}

bool StreamPusherManagement::IsExistPusher(const char *commandline)
{
	// 首先，查找进程名为ffmpeg.exe的程序
}