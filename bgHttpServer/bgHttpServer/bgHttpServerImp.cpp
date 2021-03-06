#include "bgHttpServerImp.h"
#include "bgPluginManagement.h"

#include "bgLogging.h"

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

void bgHttpServerImp::SetLogger(log4cxx::LoggerPtr *logger)
{
	logger_ = logger;
}

int bgHttpServerImp::OnInit(const char *config_ini)
{
	int errCode = 0;
	USES_CONVERSION;

	// 前期准备工作，读取配置文件
	int plugins_count = GetPrivateProfileInt(_T("SERVER_INFO"), _T("PLUGINS"), 1, A2T(config_ini));
	use_http_service_ = GetPrivateProfileInt(_T("SERVER_INFO"), _T("USE_HTTP"), 1, A2T(config_ini));
	use_https_service_ = GetPrivateProfileInt(_T("SERVER_INFO"), _T("USE_HTTPS"), 0, A2T(config_ini));

	// 创建HTTP服务监听器对象
	http_server_listener_ = ::Create_HP_HttpServerListener();

	// 创建HTTP服务器对象
	if (use_http_service_)
	{
		// 读取对应的HTTP配置
		GetPrivateProfileString(_T("HTTP_SERVICE"), _T("IP"), _T("0.0.0.0"), http_server_ip_, 4096, A2T(config_ini));
		http_port_ = (USHORT)GetPrivateProfileInt(_T("HTTP_SERVICE"), _T("PORT"), 80, A2T(config_ini));

		http_server_ = ::Create_HP_HttpServer(http_server_listener_);
		if (!http_server_)
		{
			BG_LOG_ERROR("Create_HP_HttpServer failed.");
			return -1;
		}
	}
	

	if (use_https_service_)
	{
		// 读取对应的HTTPS配置
		GetPrivateProfileString(_T("HTTPS_SERVICE"), _T("IP"), _T("0.0.0.0"), https_server_ip_, 4096, A2T(config_ini));
		https_port_ = (USHORT)GetPrivateProfileInt(_T("HTTPS_SERVICE"), _T("PORT"), 8443, A2T(config_ini));

		// 初始化SSL环境参数，测试版里面暂时把各种证书、密钥等信息写死
		https_server_ = ::Create_HP_HttpsServer(http_server_listener_);
		if (!https_server_)
		{
			BG_LOG_ERROR("Create_HP_HttpsServer failed.");
			return -2;
		}

		BOOL bret = ::HP_SSLServer_SetupSSLContext(https_server_, SSL_VM_NONE, SRV_CERT, SRV_PRI, SRV_PIN, CA_CERT, SIN_ServerNameCallback);
		if (!bret)
		{
			BG_LOG_ERROR("HP_SSLServer_SetupSSLContext failed.");
			return -3;
		}

		SPECIAL_SERVER_INDEX = ::HP_SSLServer_AddSSLContext(https_server_, SSL_VM_NONE, SRV_CERT_2, SRV_PRI_2, SRV_PIN_2, CA_CERT_2);
		if (SPECIAL_SERVER_INDEX <= 0)
		{
			BG_LOG_ERROR("HP_SSLServer_AddSSLContext failed.");
			return -4;
		}
	}
	

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
	for (int index = 0; index < plugins_count; ++index)
	{
		// 读取对应的
		char msg[4096] = {0};
		TCHAR app_name[4096] = {0};
		_stprintf_s(app_name, _T("%s_%d"), _T("PLUGINS_"), index);

		TCHAR plugin_name[4096] = {0};
		TCHAR plugin_dll[4096] = {0};

		GetPrivateProfileString(app_name, _T("NAME"), _T("DeviceControl"), plugin_name, 4096, A2T(config_ini));
		GetPrivateProfileString(app_name, _T("BINARY"), _T("bgDeviceControl.dll"), plugin_dll, 4096, A2T(config_ini));

		errCode = plugin_management_.InstallPlugin(T2A(plugin_name), T2A(plugin_dll), config_ini);
		if (errCode)
		{
			sprintf_s(msg, 4096, "Install Plugin : %s failed...", T2A(plugin_name));
			BG_LOG_ERROR(msg);
		}
		else
		{
			sprintf_s(msg, 4096, "Install Plugin : %s success...", T2A(plugin_name));
			BG_LOG_INFO(msg);
		}
	}

	return errCode;
}

int bgHttpServerImp::OnStart()
{
	int errCode = 0;
	BOOL bret = FALSE;
	char msg[4096] = {0};

	// 启动服务
	if (use_http_service_)
	{
		bret = ::HP_Server_Start(http_server_, http_server_ip_, http_port_);
		if (!bret)
		{
			// 错误描述
			USES_CONVERSION;
			LPCTSTR errstr = ::HP_Server_GetLastErrorDesc(http_server_);
			errCode = ::HP_Server_GetLastError(http_server_);
			sprintf_s(msg, 4096, "Start HTTP Server failed. : %s, errcode : %d.", T2A(errstr), errCode);
			BG_LOG_ERROR(msg);
			return errCode;
		}
	}

	if (use_https_service_)
	{
		bret = ::HP_Server_Start(https_server_, https_server_ip_, https_port_);
		if (!bret)
		{
			// 错误描述
			USES_CONVERSION;
			LPCTSTR errstr = ::HP_Server_GetLastErrorDesc(https_server_);
			errCode = ::HP_Server_GetLastError(https_server_);

			sprintf_s(msg, 4096, "Start HTTPS Server failed. : %s, errcode : %d.", T2A(errstr), errCode);
			BG_LOG_ERROR(msg);

			::HP_Server_Stop(http_server_);
		}
	}
	
	return errCode;
}

void bgHttpServerImp::OnDestroy()
{
	// 销毁各个插件

	if (use_https_service_)
	{
		::HP_Server_Stop(https_server_);
		// 清理 SSL 运行环境（可选，删除 m_HttpsServer 时会自动清理）
		::HP_SSLServer_CleanupSSLContext(https_server_);
		::Destroy_HP_HttpsServer(https_server_);

	}

	if (use_http_service_)
	{
		// 销毁 HTTP 对象
		::HP_Server_Stop(http_server_);
		::Destroy_HP_HttpServer(http_server_);
	}

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
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnAccept(HP_Server pSender, HP_CONNID dwConnID, SOCKET soClient)
{
	// 这里是指有客户端连进来的情况
	// 如果能拿到客户端的IP和端口最好
	char msg[4096] = {0};
	sprintf_s(msg, 4096, "There is a client connected.. connect_id is %d", dwConnID);
	BG_LOG_INFO(msg);
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnHandShake(HP_Server pSender, HP_CONNID dwConnID)
{
	// 握手回调
	// 什么都不做，直接返回OK
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// 这里是指接收数据
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnSend(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// 这里是指发送数据
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnClose(HP_Server pSender, HP_CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	// 这里是指关闭
	char msg[4096] = {0};
	sprintf_s(msg, 4096, "There is a client closed.. connect_id is %d", dwConnID);
	BG_LOG_INFO(msg);
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnShutdown(HP_Server pSender)
{
	// 什么都不做，直接返回OK
	return HR_OK;
}


En_HP_HttpParseResult __stdcall bgHttpServerImp::OnMessageBegin(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnRequestLine(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl)
{
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnHeader(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnHeadersComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	LPCSTR method = ::HP_HttpServer_GetMethod(pSender, dwConnID);
	LPCSTR path = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH);
	LPCSTR query = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY);
	ULONGLONG content_len = ::HP_HttpServer_GetContentLength(pSender, dwConnID);

	// 遍历所有业务插件，将消息分发进去
	bgHttpBusinessPlugins *plugin = plugin_management_.GetFirstPlugin();
	if (plugin == nullptr)
		return HPR_OK; 

	do 
	{
		if (plugin->IsMyMsg(dwConnID, method, path))
		{
			plugin->SetHttpContentLength(dwConnID, content_len);
			break;
		}

	} while ((plugin = plugin_management_.GetNextPlugin()) != nullptr);

	std::cout<<"bgHttpServerImp::OnHeadersComplete connect_id : "<<dwConnID<<std::endl;
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// 这里4096字节截断，所以各个组件需要整理头部数据以及缓存实体数据
	std::cout<<"bgHttpServerImp::OnBody connect_id : "<<dwConnID<<std::endl;

	LPCSTR method = ::HP_HttpServer_GetMethod(pSender, dwConnID);
	LPCSTR path = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH);
	LPCSTR query = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY);

	// 遍历所有业务插件，将消息分发进去
	bgHttpBusinessPlugins *plugin = plugin_management_.GetFirstPlugin();
	if (plugin == nullptr)
		return HPR_OK;

	do 
	{
		if (plugin->IsMyMsg(dwConnID, method, path))
		{
			int errCode = plugin->CacheHttpContentData(dwConnID, pData, iLength);
			if (errCode != 0)
				std::cout<<"bgHttpServerImp::OnBody cache body data failed. errCode : "<<errCode<<std::endl;

			break;
		}

	} while ((plugin = plugin_management_.GetNextPlugin()) != nullptr);

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
	char msg[4096] = {0};
	sprintf_s(msg, 4096, "There is a client request will be handled.. connect_id is %d", dwConnID);
	BG_LOG_INFO(msg);

	// 消息接收完成，取出调用的Object，分发到所有的处理插件中
	USES_CONVERSION;

	// 我们直接在这里处理相关业务
	// 至于POST的数据，应该要与ConnectID挂钩
	LPCSTR method = ::HP_HttpServer_GetMethod(pSender, dwConnID);
	LPCSTR path = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH);
	LPCSTR query = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY);

	int errCode = 0;
	unsigned char *response_data = nullptr;
	int response_len = 0;
	bool is_handled = false;

	bgHttpBusinessPlugins *plugin = plugin_management_.GetFirstPlugin();
	if (plugin == nullptr)
		return HPR_OK;

	do 
	{
		if (plugin->IsMyMsg(dwConnID, method, path))
		{
			// 处理请求
			errCode = plugin->HandleRequest(dwConnID, method, path, &response_data, &response_len, query);
			is_handled = true;

			// 处理成功，拿到返回值就返回应答
			// 实际上还应该处理cookie和session相关的内容的，这个版本就算了
			BOOL bret = ::HP_HttpServer_SendResponse(pSender, dwConnID, HSC_OK, "GoldMsg Http Server OK", nullptr, 0, response_data, response_len);
			if (!bret)
			{
				En_HP_SocketError err = ::HP_Server_GetLastError(pSender);
				LPCTSTR errstr = ::HP_GetSocketErrorDesc(err);

				sprintf_s(msg, 4096, "Send response failed. %s, errCode : %d, connect_id is %d", errstr, err, dwConnID);
				BG_LOG_WARN(msg);
			}

			// 清理应答数据
			plugin->CleanupResponseData(dwConnID, method, &response_data);

			// 如果没有保持连接的头参数，那么我们就释放掉连接
			if(!::HP_HttpServer_IsKeepAlive(pSender, dwConnID))
				::HP_HttpServer_Release(pSender, dwConnID);

			break;
		}
	} while ((plugin = plugin_management_.GetFirstPlugin()) != nullptr);

	if (!is_handled)
	{
		::HP_HttpServer_SendResponse(pSender, dwConnID, HSC_SERVICE_UNAVAILABLE, "GoldMsg Http Server", nullptr, 0, nullptr, 0);
		// 说明没有业务插件能处理这个请求，直接释放掉连接
		::HP_HttpServer_Release(pSender, dwConnID);
	}
	else
	{
		
	}

	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnUpgrade(HP_HttpServer pSender, HP_CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnParseError(HP_HttpServer pSender, HP_CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	return HPR_OK;
}


En_HP_HandleResult __stdcall bgHttpServerImp::OnWSMessageHeader(HP_HttpServer pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnWSMessageBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnWSMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	return HR_OK;
}
