#include "bgHttpServerImp.h"

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#define SPECIAL_SERVER_NAME		_T("hpsocket.org")
int SPECIAL_SERVER_INDEX = -1;

#define USE_DEFAULT_SSL_SETTTING
#ifdef USE_DEFAULT_SSL_SETTTING
#define CA_CERT		_T("bgCA.crt")		// CA根证书
#define SRV_CERT	_T("bgServer.cer")	// 服务器用的SSL公钥证书
#define SRV_PRI		_T("bgServer.key")	// 服务器用的SSL私钥证书（pkm？）
#define SRV_PIN		_T("123456")		// 服务器用的SSL私钥证书密钥

#define CA_CERT_2		_T("bgCA2.crt")		// CA根证书
#define SRV_CERT_2		_T("bgServer2.cer")	// 服务器用的SSL公钥证书
#define SRV_PRI_2		_T("bgServer2.key")	// 服务器用的SSL私钥证书（pkm？）
#define SRV_PIN_2		_T("123456")		// 服务器用的SSL私钥证书密钥
#endif

bgHttpServerImp::bgHttpServerImp()
{
	
}

bgHttpServerImp::~bgHttpServerImp()
{

}

int CALLBACK bgHttpServerImp::SIN_ServerNameCallback(LPCTSTR lpszServerName)
{
	if (::SYS_IsIPAddress(lpszServerName, nullptr))
		return 0;

	int len = lstrlen(lpszServerName);
	int diff = len - _tcslen(SPECIAL_SERVER_NAME);

	if (diff < 0)
		return 0;

	if (_tcsicmp(SPECIAL_SERVER_NAME, lpszServerName + diff) != 0)
		return 0;

	return SPECIAL_SERVER_INDEX;
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
	if (!bret)
		return -1;

	SPECIAL_SERVER_INDEX = ::HP_SSLServer_AddSSLContext(https_server_, SSL_VM_NONE, SRV_CERT_2, SRV_PRI_2, SRV_PIN_2, CA_CERT_2);
	if (SPECIAL_SERVER_INDEX <= 0)
		return -2;

	// 设置HTTP监听器回调函数
	::HP_Set_FN_HttpServer_OnPrepareListen(http_server_listener_, OnPrepareListen);
	::HP_Set_FN_HttpServer_OnAccept(http_server_listener_, OnAccept);
	::HP_Set_FN_HttpServer_OnHandShake(http_server_listener_, OnHandShake);
	::HP_Set_FN_HttpServer_OnSend(http_server_listener_, OnSend);
	::HP_Set_FN_HttpServer_OnReceive(http_server_listener_, OnReceive);
	::HP_Set_FN_HttpServer_OnClose(http_server_listener_, OnClose);
	::HP_Set_FN_HttpServer_OnShutdown(http_server_listener_, OnShutdown);

	::HP_Set_FN_HttpServer_OnMessageBegin(http_server_listener_, OnMessageBegin);
	::HP_Set_FN_HttpServer_OnRequestLine(http_server_listener_, OnRequestLine);
	::HP_Set_FN_HttpServer_OnHeader(http_server_listener_, OnHeader);
	::HP_Set_FN_HttpServer_OnHeadersComplete(http_server_listener_, OnHeadersComplete);
	::HP_Set_FN_HttpServer_OnBody(http_server_listener_, OnBody);
	::HP_Set_FN_HttpServer_OnChunkHeader(http_server_listener_, OnChunkHeader);
	::HP_Set_FN_HttpServer_OnChunkComplete(http_server_listener_, OnChunkComplete);
	::HP_Set_FN_HttpServer_OnMessageComplete(http_server_listener_, OnMessageComplete);
	::HP_Set_FN_HttpServer_OnUpgrade(http_server_listener_, OnUpgrade);
	::HP_Set_FN_HttpServer_OnParseError(http_server_listener_, OnParseError);

	::HP_Set_FN_HttpServer_OnWSMessageHeader(http_server_listener_, OnWSMessageHeader);
	::HP_Set_FN_HttpServer_OnWSMessageBody(http_server_listener_, OnWSMessageBody);
	::HP_Set_FN_HttpServer_OnWSMessageComplete(http_server_listener_, OnWSMessageComplete);

	return errCode;
}

int bgHttpServerImp::OnStart(const TCHAR *server_ip /*= _T("0.0.0.0")*/, USHORT http_port /*= 80*/, USHORT https_port /*= 8443*/)
{
	int errCode = 0;

	// 启动服务
	BOOL bret = ::HP_Server_Start(http_server_, server_ip, http_port);
	if (!bret)
	{
		// 错误描述
		::HP_Server_GetLastErrorDesc(http_server_);
		errCode = ::HP_Server_GetLastError(http_server_);
		return errCode;
	}

	bret = ::HP_Server_Start(https_server_, server_ip, https_port);
	if (!bret)
	{
		// 错误描述
		::HP_Server_GetLastErrorDesc(https_server_);
		errCode = ::HP_Server_GetLastError(https_server_);

		::HP_Server_Stop(http_server_);
	}

	return errCode;
}

void bgHttpServerImp::OnDestroy()
{
	::HP_Server_Stop(https_server_);
	::HP_Server_Stop(http_server_);

	// 清理 SSL 运行环境（可选，删除 m_HttpsServer 时会自动清理）
	::HP_SSLServer_CleanupSSLContext(http_server_);

	// 销毁 HTTP 对象
	::Destroy_HP_HttpServer(http_server_);
	::Destroy_HP_HttpsServer(https_server_);

	// 销毁监听器对象
	::Destroy_HP_HttpServerListener(http_server_listener_);
}