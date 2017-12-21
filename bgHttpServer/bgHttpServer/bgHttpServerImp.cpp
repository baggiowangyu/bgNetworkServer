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
#define CA_CERT		_T("ca.crt")		// CA��֤��
#define SRV_CERT	_T("server.cer")	// �������õ�SSL��Կ֤��
#define SRV_PRI		_T("server.key")	// �������õ�SSL˽Կ֤�飨pkm����
#define SRV_PIN		_T("123456")		// �������õ�SSL˽Կ֤����Կ

#define CA_CERT_2		_T("ca2.crt")		// CA��֤��
#define SRV_CERT_2		_T("server2.cer")	// �������õ�SSL��Կ֤��
#define SRV_PRI_2		_T("server2.key")	// �������õ�SSL˽Կ֤�飨pkm����
#define SRV_PIN_2		_T("ppmm")		// �������õ�SSL˽Կ֤����Կ
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
	USES_CONVERSION;

	// ǰ��׼����������ȡ�����ļ�
	int plugins_count = GetPrivateProfileInt(_T("SERVER_INFO"), _T("PLUGINS"), 1, A2T(config_ini));
	use_http_service_ = GetPrivateProfileInt(_T("SERVER_INFO"), _T("USE_HTTP"), 1, A2T(config_ini));
	use_https_service_ = GetPrivateProfileInt(_T("SERVER_INFO"), _T("USE_HTTPS"), 0, A2T(config_ini));

	// ����HTTP�������������
	http_server_listener_ = ::Create_HP_HttpServerListener();

	// ����HTTP����������
	if (use_http_service_)
	{
		// ��ȡ��Ӧ��HTTP����
		GetPrivateProfileString(_T("HTTP_SERVICE"), _T("IP"), _T("0.0.0.0"), http_server_ip_, 4096, A2T(config_ini));
		http_port_ = (USHORT)GetPrivateProfileInt(_T("HTTP_SERVICE"), _T("IP"), 80, A2T(config_ini));

		http_server_ = ::Create_HP_HttpServer(http_server_listener_);
	}
	

	if (use_https_service_)
	{
		// ��ȡ��Ӧ��HTTPS����
		GetPrivateProfileString(_T("HTTPS_SERVICE"), _T("IP"), _T("0.0.0.0"), https_server_ip_, 4096, A2T(config_ini));
		https_port_ = (USHORT)GetPrivateProfileInt(_T("HTTPS_SERVICE"), _T("IP"), 8443, A2T(config_ini));

		// ��ʼ��SSL�������������԰�������ʱ�Ѹ���֤�顢��Կ����Ϣд��
		https_server_ = ::Create_HP_HttpsServer(http_server_listener_);
		BOOL bret = ::HP_SSLServer_SetupSSLContext(https_server_, SSL_VM_NONE, SRV_CERT, SRV_PRI, SRV_PIN, CA_CERT, SIN_ServerNameCallback);
		if (!bret)
			return -1;

		SPECIAL_SERVER_INDEX = ::HP_SSLServer_AddSSLContext(https_server_, SSL_VM_NONE, SRV_CERT_2, SRV_PRI_2, SRV_PIN_2, CA_CERT_2);
		if (SPECIAL_SERVER_INDEX <= 0)
			return -2;
	}
	

	// ����HTTP�������ص�����
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
	// ����ҵ����
	//
	//////////////////////////////////////////////////////////////////////////
	for (int index = 0; index < plugins_count; ++index)
	{
		// ��ȡ��Ӧ��
		TCHAR app_name[4096] = {0};
		_stprintf_s(app_name, _T("%s_%d"), _T("PLUGINS_"), index);

		TCHAR plugin_name[4096] = {0};
		TCHAR plugin_dll[4096] = {0};

		GetPrivateProfileString(app_name, _T("NAME"), _T("DeviceControl"), plugin_name, 4096, A2T(config_ini));
		GetPrivateProfileString(app_name, _T("BINARY"), _T("bgDeviceControl.dll"), plugin_dll, 4096, A2T(config_ini));

		errCode = plugin_management_.InstallPlugin(T2A(plugin_name), T2A(plugin_dll), config_ini);
		if (errCode)
		{
			std::cout<<"Install Plugin : "<<T2A(plugin_name)<<" failed..."<<std::endl;
		}
	}

	return errCode;
}

int bgHttpServerImp::OnStart()
{
	int errCode = 0;
	BOOL bret = FALSE;

	// ��������
	if (use_http_service_)
	{
		bret = ::HP_Server_Start(http_server_, http_server_ip_, http_port_);
		if (!bret)
		{
			// ��������
			USES_CONVERSION;
			LPCTSTR errstr = ::HP_Server_GetLastErrorDesc(http_server_);
			errCode = ::HP_Server_GetLastError(http_server_);
			std::cout<<"Start HTTP Server failed. "<<T2A(errstr)<<". errcode : "<<errCode<<std::endl;
			return errCode;
		}
	}

	if (use_https_service_)
	{
		bret = ::HP_Server_Start(https_server_, https_server_ip_, https_port_);
		if (!bret)
		{
			// ��������
			::HP_Server_GetLastErrorDesc(https_server_);
			errCode = ::HP_Server_GetLastError(https_server_);

			::HP_Server_Stop(http_server_);
		}
	}
	
	return errCode;
}

void bgHttpServerImp::OnDestroy()
{
	if (use_https_service_)
	{
		::HP_Server_Stop(https_server_);
		// ���� SSL ���л�������ѡ��ɾ�� m_HttpsServer ʱ���Զ�����
		::HP_SSLServer_CleanupSSLContext(https_server_);
		::Destroy_HP_HttpsServer(https_server_);

	}

	if (use_http_service_)
	{
		// ���� HTTP ����
		::HP_Server_Stop(http_server_);
		::Destroy_HP_HttpServer(http_server_);
	}

	// ���ټ���������
	::Destroy_HP_HttpServerListener(http_server_listener_);
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

En_HP_HandleResult __stdcall bgHttpServerImp::OnPrepareListen(HP_Server pSender, SOCKET soListen)
{
	// ʲô��������ֱ�ӷ���OK
	std::cout<<"bgHttpServerImp::OnPrepareListen"<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnAccept(HP_Server pSender, HP_CONNID dwConnID, SOCKET soClient)
{
	// ������ָ�пͻ��������������
	std::cout<<"bgHttpServerImp::OnAccept connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnHandShake(HP_Server pSender, HP_CONNID dwConnID)
{
	// ʲô��������ֱ�ӷ���OK
	std::cout<<"bgHttpServerImp::OnHandShake connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// ������ָ��������
	std::cout<<"bgHttpServerImp::OnReceive connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnSend(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// ������ָ��������
	std::cout<<"bgHttpServerImp::OnSend connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnClose(HP_Server pSender, HP_CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	// ������ָ�ر�
	std::cout<<"bgHttpServerImp::OnClose connect_id : "<<dwConnID<<std::endl;
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnShutdown(HP_Server pSender)
{
	// ʲô��������ֱ�ӷ���OK
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
	ULONGLONG content_len = ::HP_HttpServer_GetContentLength(pSender, dwConnID);

	// ��������ҵ����������Ϣ�ַ���ȥ
	bgHttpBusinessPlugins *plugin = plugin_management_.GetFirstPlugin();

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
	// ����4096�ֽڽضϣ����Ը��������Ҫ����ͷ�������Լ�����ʵ������
	std::cout<<"bgHttpServerImp::OnBody connect_id : "<<dwConnID<<std::endl;

	LPCSTR method = ::HP_HttpServer_GetMethod(pSender, dwConnID);
	LPCSTR path = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH);
	LPCSTR query = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY);

	// ��������ҵ����������Ϣ�ַ���ȥ
	bgHttpBusinessPlugins *plugin = plugin_management_.GetFirstPlugin();

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
	std::cout<<"bgHttpServerImp::OnMessageComplete connect_id : "<<dwConnID<<std::endl;
	// ��Ϣ������ɣ�ȡ�����õ�Object���ַ������еĴ�������
	USES_CONVERSION;

	//// չʾ���е�HTTPͷ��Ϣ
	//DWORD dwHeaderCount = 0;
	//::HP_HttpServer_GetAllHeaders(pSender, dwConnID, nullptr, &dwHeaderCount);

	//if(dwHeaderCount > 0)
	//{
	//	THeader *headers = new THeader[dwHeaderCount];
	//	if(::HP_HttpServer_GetAllHeaders(pSender, dwConnID, headers, &dwHeaderCount))
	//	{
	//		for(DWORD i = 0; i < dwHeaderCount; i++)
	//			std::cout<<"client header : "<<headers[i].name<<" - "<<headers[i].value<<std::endl;
	//	}
	//	delete [] headers;
	//}

	// ����ֱ�������ﴦ�����ҵ��
	// ����POST�����ݣ�Ӧ��Ҫ��ConnectID�ҹ�
	LPCSTR method = ::HP_HttpServer_GetMethod(pSender, dwConnID);
	LPCSTR path = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_PATH);
	LPCSTR query = ::HP_HttpServer_GetUrlField(pSender, dwConnID, HUF_QUERY);

	int errCode = 0;
	unsigned char *response_data = nullptr;
	int response_len = 0;
	bool is_handled = false;

	bgHttpBusinessPlugins *plugin = plugin_management_.GetFirstPlugin();

	do 
	{
		if (plugin->IsMyMsg(dwConnID, method, path))
		{
HANDLE_REQUEST:
			// ��������
			errCode = plugin->HandleRequest(dwConnID, method, path, &response_data, &response_len, query);
			is_handled = true;

			// ����ɹ����õ�����ֵ�ͷ���Ӧ��
			// ʵ���ϻ�Ӧ�ô���cookie��session��ص����ݵģ�����汾������
			BOOL bret = ::HP_HttpServer_SendResponse(pSender, dwConnID, HSC_OK, "GoldMsg Http Server OK", nullptr, 0, response_data, response_len);
			if (!bret)
			{
				En_HP_SocketError err = ::HP_Server_GetLastError(pSender);
				LPCTSTR errstr = ::HP_GetSocketErrorDesc(err);
				std::cout<<"SendResponse failed... "<<errstr<<" errCode : "<<err<<std::endl;
			}

			// ����Ӧ������
			plugin->CleanupResponseData(dwConnID, method, &response_data);

			if (errCode == 1)
				goto HANDLE_REQUEST;

			// ���û�б������ӵ�ͷ��������ô���Ǿ��ͷŵ�����
			if(!::HP_HttpServer_IsKeepAlive(pSender, dwConnID))
				::HP_HttpServer_Release(pSender, dwConnID);

			break;
		}
	} while ((plugin = plugin_management_.GetFirstPlugin()) != nullptr);

	if (!is_handled)
	{
		::HP_HttpServer_SendResponse(pSender, dwConnID, HSC_SERVICE_UNAVAILABLE, "GoldMsg Http Server", nullptr, 0, nullptr, 0);
		// ˵��û��ҵ�����ܴ����������ֱ���ͷŵ�����
		::HP_HttpServer_Release(pSender, dwConnID);
	}
	else
	{
		
	}

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
