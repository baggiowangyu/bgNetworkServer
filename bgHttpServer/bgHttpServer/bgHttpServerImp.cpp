#include "bgHttpServerImp.h"

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#define USE_DEFAULT_SSL_SETTTING
#ifdef USE_DEFAULT_SSL_SETTTING
#define CA_CERT		_T("bgCA.crt")		// CA根证书
#define SRV_CERT	_T("bgServer.cer")	// 服务器用的SSL公钥证书
#define SRV_PRI		_T("bgServer.key")	// 服务器用的SSL私钥证书（pkm？）
#define SRV_PIN		_T("123456")		// 服务器用的SSL私钥证书密钥
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

	// 前期准备工作，读取配置文件

	// 创建HTTP服务监听器对象
	http_server_listener_ = ::Create_HP_HttpServerListener();

	// 创建HTTP服务器对象
	http_server_ = ::Create_HP_HttpServer(http_server_listener_);
	https_server_ = ::Create_HP_HttpsServer(http_server_listener_);

	// 初始化SSL环境参数，测试版里面暂时把各种证书、密钥等信息写死
	BOOL bret = ::HP_SSLServer_SetupSSLContext(https_server_, SSL_VM_NONE, SRV_CERT, SRV_PRI, SRV_PIN, CA_CERT, SIN_ServerNameCallback);

	return errCode;
}