// bgWatchDog.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>

const TCHAR *default_process = _T("bgHttpServer.exe");

int _tmain(int argc, _TCHAR* argv[])
{
	// ���ݲ��������ػ��ĸ����̣����û�д��Σ���Ĭ���ػ�bgHttpServer
	TCHAR watch_process_name[4096] = {0};

	if (argc > 1)
		_tcscpy_s(watch_process_name, 4096, argv[1]);
	else
		_tcscpy_s(watch_process_name, 4096, default_process);

	// ������һ�����գ�����bgHttpServer�Ƿ��Ѿ�������
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
			// ���ҵ�һ������ʧ����
		}
	}
	else
	{
		// �������ʧ����
	}

	while (true)
	{
		if (server_process_handle_ == nullptr)
		{
			// ��������
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
			// �ȴ�ʧ�ܣ������ǽ����˳���
			server_process_handle_ = nullptr;
			continue;
		}
		else if (dwRet == WAIT_OBJECT_0)
		{
			// �����˳���
			// ���µ��������ӿڣ�Ȼ��ǰ�����߳��˳�
			server_process_handle_ = nullptr;
			continue;
		}
		else if (WAIT_ABANDONED == dwRet)
		{
			// ��֪���Ǹ�ɶԭ��
			// ���µ��������ӿڣ�Ȼ��ǰ�����߳��˳�
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

