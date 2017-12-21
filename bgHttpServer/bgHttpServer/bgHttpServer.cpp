// bgHttpServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/command_line.h"
#include "base/file_util.h"

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

	base::FilePath config_file = current_dirctory.AppendASCII("config.ini");



	bgHttpServerImp http_server;
	int errCode = http_server.OnInit(config_file.AsUTF8Unsafe().c_str());
	errCode = http_server.OnStart();

	while (true)
	{
		Sleep(5000);
	}

	return 0;
}

