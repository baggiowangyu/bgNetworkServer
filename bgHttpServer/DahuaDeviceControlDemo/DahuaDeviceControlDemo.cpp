// DahuaDeviceControlDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "bgDaHuaDeviceControl.h"

#include <iostream>

#define TEST_PTZ_COMMAND

class StreamOb : public bgDahuaDeviceRealStreamNotifer
{
public:
	StreamOb(){}
	~StreamOb(){}

public:
	int OnInit()
	{
		int errCode = 0;

		// 初始化推流器对象

		return errCode;
	}

public:
	virtual void RealStreamDataNotify(DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, LDWORD dwUser)
	{

	}

private:
	// 推流器对象实例
};

int _tmain(int argc, _TCHAR* argv[])
{
	bgDahuaDeviceControl dahua_device_;

	std::cout<<"测试用户登录"<<std::endl;
	int errCode = dahua_device_.OnLogin("192.168.1.108", 37777, "admin", "admin");
	if (errCode != 0)
	{
		std::cout<<"登录设备失败！错误码："<<errCode<<std::endl;
		system("pause");
		return errCode;
	}
	std::cout<<"用户登录成功"<<std::endl;

	// 测试点流接口
	std::cout<<"测试实时点流"<<std::endl;
	dahua_device_.OnStartRealPlay();


#ifdef TEST_PTZ_COMMAND
	// 测试旋转接口
	std::cout<<"测试向右旋转，持续30秒"<<std::endl;
	errCode = dahua_device_.OnPTZControl_TurnRight();
	Sleep(30000);
	std::cout<<"测试向右旋转停止"<<std::endl;
	errCode = dahua_device_.OnPTZControl_TurnRight(TRUE);

	std::cout<<"测试向左旋转"<<std::endl;
	dahua_device_.OnPTZControl_TurnLeft();
	Sleep(30000);
	std::cout<<"测试向左旋转停止"<<std::endl;
	dahua_device_.OnPTZControl_TurnLeft(TRUE);

	std::cout<<"测试向上旋转"<<std::endl;
	dahua_device_.OnPTZControl_TurnUp();
	Sleep(30000);
	std::cout<<"测试向上旋转停止"<<std::endl;
	dahua_device_.OnPTZControl_TurnUp(TRUE);

	std::cout<<"测试向下旋转"<<std::endl;
	dahua_device_.OnPTZControl_TurnDown();
	Sleep(30000);
	std::cout<<"测试向下旋转停止"<<std::endl;
	dahua_device_.OnPTZControl_TurnDown(TRUE);
#endif
	
	// 维持一分钟
	//std::cout<<"实时点流预计持续1分钟"<<std::endl;
	//Sleep(60 * 1000);
	dahua_device_.OnStopRealPlay();
	
	system("pause");
	return 0;
}

