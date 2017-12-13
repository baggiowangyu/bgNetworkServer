#include "bgHttpServerImp.h"

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#define USE_DEFAULT_SSL_SETTTING
#ifdef USE_DEFAULT_SSL_SETTTING
#define CA_CERT		_T("bgCA.crt")		// CA��֤��
#define SRV_CERT	_T("bgServer.cer")	// �������õ�SSL��Կ֤��
#define SRV_PRI		_T("bgServer.key")	// �������õ�SSL˽Կ֤�飨pkm����
#define SRV_PIN		_T("123456")		// �������õ�SSL˽Կ֤����Կ
#endif

bgHttpServerImp::bgHttpServerImp()
{
	
}

bgHttpServerImp::~bgHttpServerImp()
{

}

int bgHttpServerImp::OnInit()
{
	int errCode = 0;

	// ǰ��׼����������ȡ�����ļ�

	// ����HTTP�������������
	http_server_listener_ = ::Create_HP_HttpServerListener();

	// ����HTTP����������
	http_server_ = ::Create_HP_HttpServer(http_server_listener_);
	https_server_ = ::Create_HP_HttpsServer(http_server_listener_);

	// ��ʼ��SSL�������������԰�������ʱ�Ѹ���֤�顢��Կ����Ϣд��
	BOOL bret = ::HP_SSLServer_SetupSSLContext(https_server_, SSL_VM_NONE, SRV_CERT, SRV_PRI, SRV_PIN, CA_CERT, SIN_ServerNameCallback);

	return errCode;
}