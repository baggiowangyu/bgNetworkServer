// bgConfigGenerator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	WritePrivateProfileStringA("SERVER_INFO", "PLUGINS", "123", "D:\\config.ini");
	return 0;
}

