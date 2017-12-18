#include "stdafx.h"
#include "bgHttpClientImp.h"

#include <atlconv.h>

#define CA_CERT		_T("ca.crt")
#define CLI_CERT	_T("client.cer")	// 客户端用的SSL公钥证书
#define CLI_PRI		_T("client.key")	// 客户端用的SSL私钥证书（pkm？）
#define CLI_PIN		_T("123456")		// 客户端用的SSL私钥证书密钥

bgHttpClientImp::bgHttpClientImp()
{

}

bgHttpClientImp::~bgHttpClientImp()
{

}

int bgHttpClientImp::OnInit(const char *server_ip, unsigned short server_port, bool use_https_ /* = false */)
{
	int errCode = 0;
	
	http_client_listener_ = ::Create_HP_HttpClientListener();

	// 设置 HTTP 监听器回调函数
	::HP_Set_FN_HttpClient_OnConnect(http_client_listener_, OnConnect);
	::HP_Set_FN_HttpClient_OnHandShake(http_client_listener_, OnHandShake);
	::HP_Set_FN_HttpClient_OnSend(http_client_listener_, OnSend);
	::HP_Set_FN_HttpClient_OnReceive(http_client_listener_, OnReceive);
	::HP_Set_FN_HttpClient_OnClose(http_client_listener_, OnClose);

	::HP_Set_FN_HttpClient_OnMessageBegin(http_client_listener_, OnMessageBegin);
	::HP_Set_FN_HttpClient_OnStatusLine(http_client_listener_, OnStatusLine);
	::HP_Set_FN_HttpClient_OnHeader(http_client_listener_, OnHeader);
	::HP_Set_FN_HttpClient_OnHeadersComplete(http_client_listener_, OnHeadersComplete);
	::HP_Set_FN_HttpClient_OnBody(http_client_listener_, OnBody);
	::HP_Set_FN_HttpClient_OnChunkHeader(http_client_listener_, OnChunkHeader);
	::HP_Set_FN_HttpClient_OnChunkComplete(http_client_listener_, OnChunkComplete);
	::HP_Set_FN_HttpClient_OnMessageComplete(http_client_listener_, OnMessageComplete);
	::HP_Set_FN_HttpClient_OnUpgrade(http_client_listener_, OnUpgrade);
	::HP_Set_FN_HttpClient_OnParseError(http_client_listener_, OnParseError);

	::HP_Set_FN_HttpClient_OnWSMessageHeader(http_client_listener_, OnWSMessageHeader);
	::HP_Set_FN_HttpClient_OnWSMessageBody(http_client_listener_, OnWSMessageBody);
	::HP_Set_FN_HttpClient_OnWSMessageComplete(http_client_listener_, OnWSMessageComplete);

	if (!use_https_)
		http_client_ = ::Create_HP_HttpClient(http_client_listener_);
	else
	{
		http_client_ = ::Create_HP_HttpsClient(http_client_listener_);
		if (!::HP_SSLClient_SetupSSLContext(http_client_, SSL_VM_NONE, CLI_CERT, CLI_PRI, CLI_PIN, CA_CERT))
		{
			errCode = ::HP_Client_GetLastError(http_client_);
			LPCTSTR errstr = ::HP_Client_GetLastErrorDesc(http_client_);
			return errCode;
		}
	}

	USES_CONVERSION;
	if (!::HP_Client_Start(http_client_, A2T(server_ip), server_port, FALSE))
	{
		errCode = ::HP_Client_GetLastError(http_client_);
		LPCTSTR errstr = ::HP_Client_GetLastErrorDesc(http_client_);
		return errCode;
	}

	return errCode;
}

void bgHttpClientImp::OnDestroy()
{
	::HP_Client_Stop(http_client_);
}

int bgHttpClientImp::OnSendHttpRequest(const char *path, const HP_THeader *headers /* = nullptr */, int header_count /* = 0 */, const char *method /* = "GET" */, const unsigned char *data /* = nullptr */, int data_len /* = 0 */)
{
	int errCode = 0;

	if (!::HP_HttpClient_SendRequest(http_client_, method, path, headers, header_count, (const BYTE*)data, data_len))
	{
		errCode = ::HP_Client_GetLastError(http_client_);
	}

	return errCode;
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

En_HP_HandleResult __stdcall bgHttpClientImp::OnSend(HP_Client pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	TRACE("bgHttpClientImp::OnSend connect_id : %d\n", dwConnID);
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpClientImp::OnReceive(HP_Client pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	TRACE("bgHttpClientImp::OnReceive connect_id : %d\n", dwConnID);
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpClientImp::OnClose(HP_Client pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	TRACE("bgHttpClientImp::OnClose connect_id : %d\n", dwConnID);
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpClientImp::OnConnect(HP_Client pSender, CONNID dwConnID)
{
	TRACE("bgHttpClientImp::OnConnect connect_id : %d\n", dwConnID);
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpClientImp::OnHandShake(HP_Client pSender, CONNID dwConnID)
{
	TRACE("bgHttpClientImp::OnHandShake connect_id : %d\n", dwConnID);
	return HR_OK;
}


En_HP_HttpParseResult __stdcall bgHttpClientImp::OnMessageBegin(HP_HttpClient pSender, CONNID dwConnID)
{
	TRACE("bgHttpClientImp::OnMessageBegin connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnStatusLine(HP_HttpClient pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc)
{
	TRACE("bgHttpClientImp::OnStatusLine connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnHeader(HP_HttpClient pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue)
{
	TRACE("bgHttpClientImp::OnHeader connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnHeadersComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	TRACE("bgHttpClientImp::OnHeadersComplete connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnBody(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	TRACE("bgHttpClientImp::OnBody connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnChunkHeader(HP_HttpClient pSender, CONNID dwConnID, int iLength)
{
	TRACE("bgHttpClientImp::OnChunkHeader connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnChunkComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	TRACE("bgHttpClientImp::OnChunkComplete connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnMessageComplete(HP_HttpClient pSender, CONNID dwConnID)
{
	TRACE("bgHttpClientImp::OnMessageComplete connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnUpgrade(HP_HttpClient pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType)
{
	TRACE("bgHttpClientImp::OnUpgrade connect_id : %d\n", dwConnID);
	return HPR_OK;
}

En_HP_HttpParseResult __stdcall bgHttpClientImp::OnParseError(HP_HttpClient pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc)
{
	TRACE("bgHttpClientImp::OnParseError connect_id : %d\n", dwConnID);
	return HPR_OK;
}


En_HP_HandleResult __stdcall bgHttpClientImp::OnWSMessageHeader(HP_HttpClient pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen)
{
	TRACE("bgHttpClientImp::OnWSMessageHeader connect_id : %d\n", dwConnID);
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpClientImp::OnWSMessageBody(HP_HttpClient pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	TRACE("bgHttpClientImp::OnWSMessageBody connect_id : %d\n", dwConnID);
	return HR_OK;
}

En_HP_HandleResult __stdcall bgHttpClientImp::OnWSMessageComplete(HP_HttpClient pSender, HP_CONNID dwConnID)
{
	TRACE("bgHttpClientImp::OnWSMessageComplete connect_id : %d\n", dwConnID);
	return HR_OK;
}
