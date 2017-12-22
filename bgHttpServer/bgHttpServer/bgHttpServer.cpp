// bgHttpServer.cpp : 定义控制台应用程序的入口点。
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
	// Google Chromium 基础库框架
	base::AtExitManager exit_manager;
	base::MessageLoop message_loop;

	LPTSTR cmdline = GetCommandLine();
	CommandLine commandline = CommandLine::FromString(cmdline);
	commandline.Init(argc, nullptr);

	// 这里开始业务逻辑

	// 检查是否以调试模式启动
	if (commandline.HasSwitch("debug"))
		MessageBoxA(nullptr, "等待调试器接入", "调试启动", MB_ICONWARNING|MB_OK);

	// 得到当前程序所在目录
	base::FilePath current_dirctory;
	bool bret = file_util::GetCurrentDirectory(&current_dirctory);
	if (!bret)
	{
		std::cout<<"Get current directory failed..."<<std::endl;
		return -1;
	}

	// 初始化日志模块
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

	// 初始化HTTP服务器实体类
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

