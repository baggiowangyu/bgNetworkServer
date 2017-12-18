#ifndef _BG_HTTP_CLIENT_IMP_H_
#define _BG_HTTP_CLIENT_IMP_H_

#include "HPSocket4C-SSL.h"

class bgHttpClientImp
{
public:
	bgHttpClientImp();
	~bgHttpClientImp();

public:
	static En_HP_HandleResult __stdcall OnSend(HP_Client pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnReceive(HP_Client pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnClose(HP_Client pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	static En_HP_HandleResult __stdcall OnConnect(HP_Client pSender, CONNID dwConnID);
	static En_HP_HandleResult __stdcall OnHandShake(HP_Client pSender, CONNID dwConnID);

	static En_HP_HttpParseResult __stdcall OnMessageBegin(HP_HttpClient pSender, CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnStatusLine(HP_HttpClient pSender, CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc);
	static En_HP_HttpParseResult __stdcall OnHeader(HP_HttpClient pSender, CONNID dwConnID, LPCSTR lpszName, LPCSTR lpszValue);
	static En_HP_HttpParseResult __stdcall OnHeadersComplete(HP_HttpClient pSender, CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnBody(HP_HttpClient pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HttpParseResult __stdcall OnChunkHeader(HP_HttpClient pSender, CONNID dwConnID, int iLength);
	static En_HP_HttpParseResult __stdcall OnChunkComplete(HP_HttpClient pSender, CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnMessageComplete(HP_HttpClient pSender, CONNID dwConnID);
	static En_HP_HttpParseResult __stdcall OnUpgrade(HP_HttpClient pSender, CONNID dwConnID, EnHttpUpgradeType enUpgradeType);
	static En_HP_HttpParseResult __stdcall OnParseError(HP_HttpClient pSender, CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc);

	static En_HP_HandleResult __stdcall OnWSMessageHeader(HP_HttpClient pSender, HP_CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen);
	static En_HP_HandleResult __stdcall OnWSMessageBody(HP_HttpClient pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength);
	static En_HP_HandleResult __stdcall OnWSMessageComplete(HP_HttpClient pSender, HP_CONNID dwConnID);

public:
	int OnInit(const char *server_ip, unsigned short server_port, bool use_https_ = false);
	void OnDestroy();

public:
	int OnSendHttpRequest(const char *path, const HP_THeader *headers = nullptr, int header_count = 0, const char *method = "GET", const unsigned char *data = nullptr, int data_len = 0);

public:
	HP_HttpClient http_client_;
	HP_HttpClientListener http_client_listener_;
};

#endif//_BG_HTTP_CLIENT_IMP_H_
