#ifndef _BG_HTTP_SERVER_IMP_H_
#define _BG_HTTP_SERVER_IMP_H_

#define _HTTP_SUPPORT
#define _SSL_SUPPORT

#include <atlstr.h>

#include "HPSocket4C-SSL.h"

class bgHttpServerImp
{
public:
	bgHttpServerImp();
	~bgHttpServerImp();

public:
	static En_HP_HandleResult __stdcall OnPrepareListen(HP_Server pSender, SOCKET soListen);
	static En_HP_HandleResult __stdcall OnAccept(HP_Server pSender, HP_CONNID dwConnID, SOCKET soClient);
	static En_HP_HandleResult __stdcall OnHandShake(HP_Server pSender, HP_CONNID dwConnID);
	static En_HP_HandleResult __stdcall OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnSend(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnClose(HP_Server pSender, HP_CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	static En_HP_HandleResult __stdcall OnShutdown(HP_Server pSender);

	static En_HP_HttpParseResult __stdcall OnMessageBegin(HP_HttpServer pSender, HP_CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnRequestLine(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszMethod, LPCSTR lpszUrl);
	static En_HP_HttpParseResult __stdcall OnHeader(HP_HttpServer pSender, HP_CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	static En_HP_HttpParseResult __stdcall OnHeadersComplete(HP_HttpServer pSender, HP_CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HttpParseResult __stdcall OnChunkHeader(HP_HttpServer pSender, HP_CONNID dwConnID, int iLength);
	static En_HP_HttpParseResult __stdcall OnChunkComplete(HP_HttpServer pSender, HP_CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnUpgrade(HP_HttpServer pSender, HP_CONNID dwConnID, EnHttpUpgradeType enUpgradeType);
	static En_HP_HttpParseResult __stdcall OnParseError(HP_HttpServer pSender, HP_CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc);

	static En_HP_HandleResult __stdcall OnWSMessageHeader(HP_HttpServer pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
	static En_HP_HandleResult __stdcall OnWSMessageBody(HP_HttpServer pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnWSMessageComplete(HP_HttpServer pSender, HP_CONNID dwConnID);

public:
	static int CALLBACK SIN_ServerNameCallback(LPCTSTR lpszServerName);

public:
	int OnInit(const char *config_ini);
	int OnStart();
	void OnDestroy();

public:
	HP_HttpServer http_server_;
	HP_HttpsServer https_server_;

	HP_HttpServerListener http_server_listener_;

public:
	int use_http_service_;
	TCHAR http_server_ip_[4096];
	USHORT http_port_;

	int use_https_service_;
	TCHAR https_server_ip_[4096];
	USHORT https_port_;
};

#endif//_BG_HTTP_SERVER_IMP_H_