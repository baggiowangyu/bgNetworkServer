#include "StreamPusherManagement.h"
#include "winternl.h"

#include <tchar.h>
#include <Tlhelp32.h>


typedef NTSTATUS  (WINAPI *NtQueryInformationProcessFake)(HANDLE, DWORD, PVOID, ULONG, PULONG);  
NtQueryInformationProcessFake ntQ = NULL;  

bool getProcCMD(DWORD pid, wchar_t cmdline_buffer[409600])
{
	bool b = false;
	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid );
	if (INVALID_HANDLE_VALUE != process_handle)
	{
		HANDLE hnewdup = NULL;
		PEB peb;
		RTL_USER_PROCESS_PARAMETERS upps;

		HMODULE hm = LoadLibrary(_T("Ntdll.dll"));
		ntQ = (NtQueryInformationProcessFake)GetProcAddress(hm, "NtQueryInformationProcess");

		BOOL bret = DuplicateHandle(GetCurrentProcess(), process_handle, GetCurrentProcess(), &hnewdup, 0, FALSE, DUPLICATE_SAME_ACCESS);
		if (!bret)
		{
			CloseHandle(process_handle);
			return b;
		}

		PROCESS_BASIC_INFORMATION pbi;
		NTSTATUS isok = ntQ(hnewdup, 0/*ProcessBasicInformation*/, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), 0);
		if (BCRYPT_SUCCESS(isok))
		{
			if ( ReadProcessMemory(hnewdup, pbi.PebBaseAddress, &peb, sizeof(PEB), 0))
				if ( ReadProcessMemory(hnewdup, peb.ProcessParameters, &upps, sizeof(RTL_USER_PROCESS_PARAMETERS), 0))
				{
					ZeroMemory(cmdline_buffer, (upps.CommandLine.Length + 1) * sizeof(WCHAR));
					ReadProcessMemory(hnewdup, upps.CommandLine.Buffer, cmdline_buffer, upps.CommandLine.Length, 0);
					b = true;
				}
		}
		CloseHandle(hnewdup);
	}

	return b;
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
		sprintf_s(param, 4096, "-i %s -vcodec copy -acodec copy -rtsp_transport tcp -f rtsp %s://%s:%s/%s", source_url, protocol, target_ip, target_port, stream_name);
	else
		return ERROR_NOT_SUPPORTED;

	// ��鵱ǰ�Ƿ����һ��ffmpeg����ִ��������ÿ������������ffmpeg�����費ͬ���ļ���
	// �ҵ��ý��̣���ѯ�����У���������в���������Ҫִ�е�һ�£���ô����ֻҪ�ػ�ס��Ӧ�Ľ��̾������
	// ���û���ҵ�һ�µģ���ô���Ǿʹ���һ���µ���������
	if (IsExistPusher(param))
	{
		// ʲô������
	}
	else
	{
		// ������������������Ȼ���߳�����������
		// ��������˳�������
		memset(&shell_info_, 0, sizeof(SHELLEXECUTEINFOA));
		shell_info_.cbSize = sizeof(SHELLEXECUTEINFOA);
		shell_info_.fMask = SEE_MASK_NOCLOSEPROCESS;
		shell_info_.lpFile = "bgCarStreamPusher.exe";
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

bool StreamPusherManagement::IsExistPusher(const char *commandline)
{
	bool bret = false;
	// ���ȣ����ҽ�����ΪbgCarStreamPusher.exe�ĳ���
	// �ҵõ�˵����������

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapShot == INVALID_HANDLE_VALUE)
		return bret;

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))
		return bret;

	do 
	{
		TCHAR *process_name = pe.szExeFile;

		if (_tcsicmp(process_name, _T("bgCarStreamPusher.exe")) == 0)
		{
			bret = true;

			// �ҵ��˶�Ӧ�Ľ��̣�Ȼ������ȡ
			DWORD process_id = pe.th32ProcessID;
			wchar_t commandline[409600] = {0};
			BOOL bret = getProcCMD(process_id, commandline);
			if (!bret)
				break;

			// ������֤������������Ƿ��ǲ�ѯ���������е��Ӽ�

			// ��֤ͨ�����򿪽��̾���������������������ǲ�����СȨ�ޣ����ȴ�Ȩ��
			pusher_handle_ = OpenProcess(SYNCHRONIZE, FALSE, process_id);

			break;
		}
	} while (Process32Next(hSnapShot, &pe));

	return bret;
}