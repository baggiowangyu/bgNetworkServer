#include "bgHttpServerImp.h"
#include "bgPluginManagement.h"

#include <iostream>
#include <atlconv.h>

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

#define SPECIAL_SERVER_NAME		_T("hpsocket.org")
int SPECIAL_SERVER_INDEX = -1;

#define USE_DEFAULT_SSL_SETTTING
#ifdef USE_DEFAULT_SSL_SETTTING
#define CA_CERT		_T("ca.crt")		// CA根证书
#define SRV_CERT	_T("server.cer")	// 服务器用的SSL公钥证书
#define SRV_PRI		_T("server.key")	// 服务器用的SSL私钥证书（pkm？）
#define SRV_PIN		_T("123456")		// 服务器用的SSL私钥证书密钥

#define CA_CERT_2		_T("ca2.crt")		// CA根证书
#define SRV_CERT_2		_T("server2.cer")	// 服务器用的SSL公钥证书
#define SRV_PRI_2		_T("server2.key")	// 服务器用的SSL私钥证书（pkm？）
#define SRV_PIN_2		_T("ppmm")		// 服务器用的SSL私钥证书密钥
#endif

bgPluginManagement plugin_management_;

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

int bgHttpServerImp::OnInit(const char *config_ini)
{
	int errCode = 0;

	// 前期准备工作，读取配置文件
	

	// 创建HTTP服务监听器对象
	http_server_listener_ = ::Create_HP_HttpServerListener();

	// 创建HTTP服务器对象
	http_server_ = ::Create_HP_HttpServer(http_server_listener_);
	
#ifdef USE_HTTPS
	// 初始化SSL环境参数，测试版里面暂时把各种证书、密钥等信息写死
	https_server_ = ::Create_HP_HttpsServer(http_server_listener_);
	BOOL bret = ::HP_SSLServer_SetupSSLContext(https_server_, SSL_VM_NONE, SRV_CERT, SRV_PRI, SRV_PIN, CA_CERT, SIN_ServerNameCallback);
	if (!bret)
		return -1;

	SPECIAL_SERVER_INDEX = ::HP_SSLServer_AddSSLContext(https_server_, SSL_VM_NONE, SRV_CERT_2, SRV_PRI_2, SRV_PIN_2, CA_CERT_2);
	if (SPECIAL_SERVER_INDEX <= 0)
		return -2;
#endif

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

	//////////////////////////////////////////////////////////////////////////
	//
	// 加载业务插件
	//
	//////////////////////////////////////////////////////////////////////////
	

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
		USES_CONVERSION;
		LPCTSTR errstr = ::HP_Server_GetLastErrorDesc(http_server_);
		errCode = ::HP_Server_GetLastError(http_server_);
		std::cout<<"Start HTTP Server failed. "<<T2A(errstr)<<". errcode : "<<errCode<<std::endl;
		return errCode;
	}

#ifdef USE_HTTPS
	bret = ::HP_Server_Start(https_server_, server_ip, https_port);
	if (!bret)
	{
		// 错误描述
		::HP_Server_GetLastErrorDesc(https_server_);
		errCode = ::HP_Server_GetLastError(https_server_);

		::HP_Server_Stop(http_server_);
	}
#endif

	return errCode;
}

void bgHttpServerImp::OnDestroy()
{
#ifdef USE_HTTPS
	::HP_Server_Stop(https_server_);
#endif
	::HP_Server_Stop(http_server_);

	// 清理 SSL 运行环境（可选，删除 m_HttpsServer 时会自动清理）
	::HP_SSLServer_CleanupSSLContext(http_server_);

	// 销毁 HTTP 对象
	::Destroy_HP_HttpServer(http_server_);
#ifdef USE_HTTPS
	::Destroy_HP_HttpsServer(https_server_);
#endif

	// 销毁监听器对象
	::Destroy_HP_HttpServerListener(http_server_listener_);
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

En_HP_HandleResult __stdcall bgHttpServerImp::OnPrepareListen(HP_Server pSender, SOCKET soListen)
{
	// 什么都不做，直接返回OK
	std::cout<<"bgHttpServerImp::OnPrepareListen"<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnAccept(HP_Server pSender, HP_CONNID dwConnID, SOCKET soClient)
{
	// 这里是指有客户端连进来的情况
	std::cout<<"bgHttpServerImp::OnAccept connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnHandShake(HP_Server pSender, HP_CONNID dwConnID)
{
	// 什么都不做，直接返回OK
	std::cout<<"bgHttpServerImp::OnHandShake connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// 这里是指接收数据
	std::cout<<"bgHttpServerImp::OnReceive connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnSend(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// 这里是指发送数据
	std::cout<<"bgHttpServerImp::OnSend connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnClose(HP_Server pSender, HP_CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	// 这里是指关闭
	std::cout<<"bgHttpServerImp::OnClose connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnShutdown(HP_Server pSender)
{
	// 什么都不做，直接返回OK
	std::cout<<"bgHttpServerImp::OnShutdown"<<std::endl;
	return HR_OK;
}


En_HP_HttpParseResult __stdcall bgHttpServerImp::OnMessageBegin(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	std::cout<<"bgHttpServerImp::OnMessageBegin connect_id : "<<dwConnID<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnRequestLine(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
{
	std::cout<<"bgHttpServerImp::OnRequestLine connect_id : "<<dwConnID<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnHeader(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	std::cout<<"bgHttpServerImp::OnHeader connect_id : "<<dwConnID<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnHeadersComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	LPCSTR method = ::HP_HttpServer_GetMethod(pSender, dwConnID);
	LPCSTR path = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH);
	LPCSTR query = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY);

	// 遍历所有业务插件，将消息分发进去
	bgHttpBusinessPlugins *plugin = plugin_management_.GetFirstPlugin();

	do 
	{
		plugin->IsMyMsg(dwConnID, method, path);

	} while ((plugin = plugin_management_.GetNextPlugin()) != nullptr);

	std::cout<<"bgHttpServerImp::OnHeadersComplete connect_id : "<<dwConnID<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// 这里4096字节截断，所以各个组件需要整理头部数据以及缓存实体数据
	std::cout<<"bgHttpServerImp::OnBody Start====================="<<std::endl;
	std::cout<<"bgHttpServerImp::OnBody connect_id : "<<dwConnID<<std::endl;

	std::cout<<"data_len : "<<iLength<<std::endl;
	char *buffer = new char[iLength + 1];
	memcpy(buffer, pData, iLength);
	buffer[iLength] = '\0';
	std::cout<<"data : "<<buffer<<std::endl;
	std::cout<<"bgHttpServerImp::OnBody Finished====================="<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnChunkHeader(HP_HttpServer pSender, HP_CONNID dwConnID, int iLength)
{
	std::cout<<"bgHttpServerImp::OnChunkHeader connect_id : "<<dwConnID<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnChunkComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	std::cout<<"bgHttpServerImp::OnChunkComplete"<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	// 消息接收完成，取出调用的Object，分发到所有的处理插件中
	USES_CONVERSION;
	std::cout<<"bgHttpServerImp::OnMessageComplete Start ==========="<<std::endl;
	std::cout<<"connect_id : "<<dwConnID<<std::endl;
	std::cout<<"client request method : "<<::HP_HttpServer_GetMethod(pSender, dwConnID)<<std::endl;
	std::cout<<"client schema : "<<::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_SCHEMA)<<std::endl;
	std::cout<<"client host : "<<::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_HOST)<<std::endl;
	std::cout<<"client port : "<<::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PORT)<<std::endl;
	std::cout<<"client path : "<<::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH)<<std::endl;
	std::cout<<"client query : "<<::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY)<<std::endl;
	std::cout<<"client fragment : "<<::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_FRAGMENT)<<std::endl;
	std::cout<<"client userinfo : "<<::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_USERINFO)<<std::endl;

	// 展示所有的HTTP头信息
	DWORD dwHeaderCount = 0;
	::HP_HttpServer_GetAllHeaders(pSender, dwConnID, nullptr, &dwHeaderCount);

	if(dwHeaderCount > 0)
	{
		THeader *headers = new THeader[dwHeaderCount];
		if(::HP_HttpServer_GetAllHeaders(pSender, dwConnID, headers, &dwHeaderCount))
		{
			for(DWORD i = 0; i < dwHeaderCount; i++)
				std::cout<<"client header : "<<headers[i].name<<" - "<<headers[i].value<<std::endl;
		}
		delete [] headers;
	}

	// 展示传递过来的数据信息
	std::cout<<"client content_type : "<<((::HP_HttpServer_GetContentType(pSender, dwConnID) == NULL) ? "None" : (::HP_HttpServer_GetContentType(pSender, dwConnID)))<<std::endl;
	std::cout<<"client content_length : "<<::HP_HttpServer_GetContentLength(pSender, dwConnID)<<std::endl;
	std::cout<<"client content_encoding : "<<((::HP_HttpServer_GetContentEncoding(pSender, dwConnID) == NULL) ? "None" : (::HP_HttpServer_GetContentEncoding(pSender, dwConnID)))<<std::endl;

	std::cout<<"bgHttpServerImp::OnMessageComplete Finish ==========="<<std::endl;

	// 我们直接在这里处理相关业务
	// 至于POST的数据，应该要与ConnectID挂钩
	LPCSTR method = ::HP_HttpServer_GetMethod(pSender, dwConnID);
	LPCSTR path = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH);
	LPCSTR query = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY);

	bgHttpBusinessPlugins *plugin = plugin_management_.GetFirstPlugin();

	do 
	{
		if (plugin->IsMyMsg(dwConnID, method, path))
		{
			// 处理请求
			plugin->HandleRequest(dwConnID, method, path, query);
		}
	} while ((plugin = plugin_management_.GetFirstPlugin()) != nullptr);

	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnUpgrade(HP_HttpServer pSender, HP_CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	std::cout<<"bgHttpServerImp::OnUpgrade connect_id : "<<dwConnID<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnParseError(HP_HttpServer pSender, HP_CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	std::cout<<"bgHttpServerImp::OnParseError connect_id : "<<dwConnID<<std::endl;
	return HPR_OK;
}


En_HP_HandleResult __stdcall bgHttpServerImp::OnWSMessageHeader(HP_HttpServer pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	std::cout<<"bgHttpServerImp::OnWSMessageHeader connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnWSMessageBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	std::cout<<"bgHttpServerImp::OnWSMessageBody connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnWSMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	std::cout<<"bgHttpServerImp::OnWSMessageComplete connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}
