
// bgCarEndpoint.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CbgCarEndpointApp:
// �йش����ʵ�֣������ bgCarEndpoint.cpp
//

class CbgCarEndpointApp : public CWinApp
{
public:
	CbgCarEndpointApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CbgCarEndpointApp theApp;