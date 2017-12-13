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
#define CA_CERT		_T("bgCA.crt")		// CA��֤��
#define SRV_CERT	_T("bgServer.cer")	// �������õ�SSL��Կ֤��
#define SRV_PRI		_T("bgServer.key")	// �������õ�SSL˽Կ֤�飨pkm����
#define SRV_PIN		_T("123456")		// �������õ�SSL˽Կ֤����Կ

#define CA_CERT_2		_T("bgCA2.crt")		// CA��֤��
#define SRV_CERT_2		_T("bgServer2.cer")	// �������õ�SSL��Կ֤��
#define SRV_PRI_2		_T("bgServer2.key")	// �������õ�SSL˽Կ֤�飨pkm����
#define SRV_PIN_2		_T("123456")		// �������õ�SSL˽Կ֤����Կ
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

	// ǰ��׼����������ȡ�����ļ�

	// ����HTTP�������������
	http_server_listener_ = ::Create_HP_HttpServerListener();

	// ����HTTP����������
	http_server_ = ::Create_HP_HttpServer(http_server_listener_);
	https_server_ = ::Create_HP_HttpsServer(http_server_listener_);

	// ��ʼ��SSL�������������԰�������ʱ�Ѹ���֤�顢��Կ����Ϣд��
	BOOL bret = ::HP_SSLServer_SetupSSLContext(https_server_, SSL_VM_NONE, SRV_CERT, SRV_PRI, SRV_PIN, CA_CERT, SIN_ServerNameCallback);
	if (!bret)
		return -1;

	SPECIAL_SERVER_INDEX = ::HP_SSLServer_AddSSLContext(https_server_, SSL_VM_NONE, SRV_CERT_2, SRV_PRI_2, SRV_PIN_2, CA_CERT_2);
	if (SPECIAL_SERVER_INDEX <= 0)
		return -2;

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

	return errCode;
}

int bgHttpServerImp::OnStart(const TCHAR *server_ip /*= _T("0.0.0.0")*/, USHORT http_port /*= 80*/, USHORT https_port /*= 8443*/)
{
	int errCode = 0;

	// ��������
	BOOL bret = ::HP_Server_Start(http_server_, server_ip, http_port);
	if (!bret)
	{
		// ��������
		::HP_Server_GetLastErrorDesc(http_server_);
		errCode = ::HP_Server_GetLastError(http_server_);
		return errCode;
	}

	bret = ::HP_Server_Start(https_server_, server_ip, https_port);
	if (!bret)
	{
		// ��������
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

	// ���� SSL ���л�������ѡ��ɾ�� m_HttpsServer ʱ���Զ�����
	::HP_SSLServer_CleanupSSLContext(http_server_);

	// ���� HTTP ����
	::Destroy_HP_HttpServer(http_server_);
	::Destroy_HP_HttpsServer(https_server_);

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
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnAccept(HP_Server pSender, HP_CONNID dwConnID, SOCKET soClient)
{
	// ������ָ�пͻ��������������
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnHandShake(HP_Server pSender, HP_CONNID dwConnID)
{
	// ʲô��������ֱ�ӷ���OK
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// ������ָ��������
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnSend(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	// ������ָ��������
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnClose(HP_Server pSender, HP_CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	// ������ָ�ر�
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpServerImp::OnShutdown(HP_Server pSender)
{
	// ʲô��������ֱ�ӷ���OK
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
	// ����HTTPͷ���Ѿ�������ɣ�����ͨ��
	// unique_ptr<THeader[]> headers(new THeader[dwHeaderCount]);
	// VERIFY(::HP_HttpServer_GetAllHeaders(pSender, dwConnID, headers.get(), &dwHeaderCount));
	// ��ȡ����ͷ������

	// unique_ptr<TCookie[]> cookies(new TCookie[dwCookieCount]);
	// VERIFY(::HP_HttpServer_GetAllCookies(pSender, dwConnID, cookies.get(), &dwCookieCount));
	// ��ȡ����cookie
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnChunkHeader(HP_HttpServer pSender, HP_CONNID dwConnID, int iLength)
{
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnChunkComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpServerImp::OnMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID)
{
	// ������HTTP��Ϣ��������
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
