// bgHttpServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "bgHttpServerImp.h"


int _tmain(int argc, _TCHAR* argv[])
{
	bgHttpServerImp http_server;
	int errCode = http_server.OnInit("config.ini");
	errCode = http_server.OnStart(_T("0.0.0.0"), 9876, 8765);

	while (true)
	{
		Sleep(5000);
	}

	return 0;
}

