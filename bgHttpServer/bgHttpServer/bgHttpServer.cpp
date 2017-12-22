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

	// ��ʼ����־ģ��
	base::Time current_time = base::Time::Now();
	base::Time::Exploded time;
	current_time.UTCExplode(&time);
	std::string log_file_name = base::StringPrintf("log_%d-%02d-%02d_%02d%02d%02d.log", 
		time.year, time.month, time.day_of_month, time.hour, time.minute, time.second);
	base::FilePath log_directory = current_dirctory.AppendASCII("log");
	base::FilePath log_file_path = log_directory.AppendASCII(log_file_name);

	//logging::InitLogging(base::SysUTF8ToWide(log_file_path.AsUTF8Unsafe()).c_str(),
	//	logging::LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG, logging::DONT_LOCK_LOG_FILE,
	//	logging::APPEND_TO_OLD_LOG_FILE, logging::ENABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);

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

