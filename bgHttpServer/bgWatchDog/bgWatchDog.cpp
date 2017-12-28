// bgWatchDog.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>

const TCHAR *default_process = _T("bgHttpServer.exe");

int _tmain(int argc, _TCHAR* argv[])
{
	// 根据参数决定守护哪个进程，如果没有传参，则默认守护bgHttpServer
	TCHAR watch_process_name[4096] = {0};

	if (argc > 1)
		_tcscpy_s(watch_process_name, 4096, argv[1]);
	else
		_tcscpy_s(watch_process_name, 4096, default_process);

	// 首先拍一个快照，看看bgHttpServer是否已经启动了
	HANDLE server_process_handle_ = nullptr;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapShot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapShot, &pe))
		{
			do 
			{
				TCHAR *process_name = pe.szExeFile;

				if (_tcsicmp(process_name, watch_process_name) == 0)
				{
					DWORD process_id = pe.th32ProcessID;
					server_process_handle_ = OpenProcess(SYNCHRONIZE, FALSE, process_id);

					break;
				}
			} while (Process32Next(hSnapShot, &pe));
		}
		else
		{
			// 查找第一个进程失败了
		}
	}
	else
	{
		// 拍摄快照失败了
	}

	while (true)
	{
		if (server_process_handle_ == nullptr)
		{
			// 启动服务
			SHELLEXECUTEINFO shell_info_;
			memset(&shell_info_, 0, sizeof(SHELLEXECUTEINFO));
			shell_info_.cbSize = sizeof(SHELLEXECUTEINFO);
			shell_info_.fMask = SEE_MASK_NOCLOSEPROCESS;
			shell_info_.lpFile = watch_process_name;
			shell_info_.lpVerb = _T("runas");
			shell_info_.nShow = SW_MINIMIZE;

			BOOL bret = ShellExecuteEx(&shell_info_);
			if (bret)
				server_process_handle_ = shell_info_.hProcess;
		}

		DWORD dwRet = WaitForSingleObject(server_process_handle_, INFINITE);
		if (dwRet == WAIT_FAILED)
		{
			// 等待失败，可能是进程退出了
			server_process_handle_ = nullptr;
			continue;
		}
		else if (dwRet == WAIT_OBJECT_0)
		{
			// 进程退出了
			// 重新调用推流接口，然后当前工作线程退出
			server_process_handle_ = nullptr;
			continue;
		}
		else if (WAIT_ABANDONED == dwRet)
		{
			// 不知道是个啥原因
			// 重新调用推流接口，然后当前工作线程退出
			server_process_handle_ = nullptr;
			continue;
		}
		else if (WAIT_TIMEOUT == dwRet)
		{
			server_process_handle_ = nullptr;
			continue;
		}
		else
		{
			server_process_handle_ = nullptr;
			continue;
		}

		Sleep(500);
	}
	
	return 0;
}

