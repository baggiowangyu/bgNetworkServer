// DahuaDeviceControlDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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

		// ��ʼ������������

		return errCode;
	}

public:
	virtual void RealStreamDataNotify(DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG param, LDWORD dwUser)
	{

	}

private:
	// ����������ʵ��
};

int _tmain(int argc, _TCHAR* argv[])
{
	bgDahuaDeviceControl dahua_device_;

	std::cout<<"�����û���¼"<<std::endl;
	int errCode = dahua_device_.OnLogin("192.168.1.108", 37777, "admin", "admin");
	if (errCode != 0)
	{
		std::cout<<"��¼�豸ʧ�ܣ������룺"<<errCode<<std::endl;
		system("pause");
		return errCode;
	}
	std::cout<<"�û���¼�ɹ�"<<std::endl;

	// ���Ե����ӿ�
	std::cout<<"����ʵʱ����"<<std::endl;
	dahua_device_.OnStartRealPlay();


#ifdef TEST_PTZ_COMMAND
	// ������ת�ӿ�
	std::cout<<"����������ת������30��"<<std::endl;
	errCode = dahua_device_.OnPTZControl_TurnRight();
	Sleep(30000);
	std::cout<<"����������תֹͣ"<<std::endl;
	errCode = dahua_device_.OnPTZControl_TurnRight(TRUE);

	std::cout<<"����������ת"<<std::endl;
	dahua_device_.OnPTZControl_TurnLeft();
	Sleep(30000);
	std::cout<<"����������תֹͣ"<<std::endl;
	dahua_device_.OnPTZControl_TurnLeft(TRUE);

	std::cout<<"����������ת"<<std::endl;
	dahua_device_.OnPTZControl_TurnUp();
	Sleep(30000);
	std::cout<<"����������תֹͣ"<<std::endl;
	dahua_device_.OnPTZControl_TurnUp(TRUE);

	std::cout<<"����������ת"<<std::endl;
	dahua_device_.OnPTZControl_TurnDown();
	Sleep(30000);
	std::cout<<"����������תֹͣ"<<std::endl;
	dahua_device_.OnPTZControl_TurnDown(TRUE);
#endif
	
	// ά��һ����
	//std::cout<<"ʵʱ����Ԥ�Ƴ���1����"<<std::endl;
	//Sleep(60 * 1000);
	dahua_device_.OnStopRealPlay();
	
	system("pause");
	return 0;
}

