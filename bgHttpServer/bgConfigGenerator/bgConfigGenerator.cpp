// bgConfigGenerator.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	WritePrivateProfileStringA("SERVER_INFO", "PLUGINS", "123", "D:\\config.ini");
	return 0;
}

