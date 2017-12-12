#ifndef _BG_HTTP_SERVER_IMP_H_
#define _BG_HTTP_SERVER_IMP_H_

#define _HTTP_SUPPORT
#define _SSL_SUPPORT

#include "HttpServer.h"
#include "Global/helper.h"

#include <atlstr.h>

class CHttpServerListenerImpl : public CHttpServerListener
{
private:

public:
	CHttpServerListenerImpl()
	{
	}
};


class bgHttpServerImp
{
public:
	bgHttpServerImp();
	~bgHttpServerImp();

public:
	int OnInit();

public:
	CHttpServerListenerImpl m_HttpListener;
	CHttpServerListenerImpl m_HttpsListener;

	CHttpServer m_HttpServer;
	CHttpsServer m_HttpsServer;
};

#endif//_BG_HTTP_SERVER_IMP_H_