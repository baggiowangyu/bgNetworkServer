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

#include "log4cxx/logger.h"
#include "log4cxx/logstring.h"
#include "log4cxx/propertyconfigurator.h"

#include "bgLogging.h"
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

	// 打开一个互斥体，确保是单例模式
	HANDLE mutex_handle = CreateMutex(nullptr, TRUE, _T("__GM_bgHttpServer__"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// 已经存在一个实例了，当前服务进程退出
		return 0;
	}

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

	// 读取日志模块配置文件
	base::FilePath log4cxx_properties = current_dirctory.AppendASCII("log4cxx.properties");
	BG_LOG_INIT(log4cxx_properties.value());

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

