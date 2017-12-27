// bgHttpServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/command_line.h"
#include "base/file_util.h"
#include "base/time.h"
#include "base/stringprintf.h"
#include "base/strings/sys_string_conversions.h"
#include "base/logging.h"

#include "log4cxx/logger.h"
#include "log4cxx/logstring.h"
#include "log4cxx/propertyconfigurator.h"

#include "bgLogging.h"
#include "bgHttpServerImp.h"

#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	// Google Chromium ��������
	base::AtExitManager exit_manager;
	base::MessageLoop message_loop;

	LPTSTR cmdline = GetCommandLine();
	CommandLine commandline = CommandLine::FromString(cmdline);
	commandline.Init(argc, nullptr);

	// ���￪ʼҵ���߼�

	// ��һ�������壬ȷ���ǵ���ģʽ
	HANDLE mutex_handle = CreateMutex(nullptr, TRUE, _T("__GM_bgHttpServer__"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// �Ѿ�����һ��ʵ���ˣ���ǰ��������˳�
		return 0;
	}

	// ����Ƿ��Ե���ģʽ����
	if (commandline.HasSwitch("debug"))
		MessageBoxA(nullptr, "�ȴ�����������", "��������", MB_ICONWARNING|MB_OK);

	// �õ���ǰ��������Ŀ¼
	base::FilePath current_dirctory;
	bool bret = file_util::GetCurrentDirectory(&current_dirctory);
	if (!bret)
	{
		std::cout<<"Get current directory failed..."<<std::endl;
		return -1;
	}

	// ��ȡ��־ģ�������ļ�
	base::FilePath log4cxx_properties = current_dirctory.AppendASCII("log4cxx.properties");
	BG_LOG_INIT(log4cxx_properties.value());

	// ��ʼ��HTTP������ʵ����
	base::FilePath config_file = current_dirctory.AppendASCII("config.ini");
	std::string config_path = config_file.AsUTF8Unsafe();
	bgHttpServerImp http_server;
	int errCode = http_server.OnInit(config_path.c_str());
	errCode = http_server.OnStart();

	while (true)
	{
		Sleep(5000);
	}

	return 0;
}

