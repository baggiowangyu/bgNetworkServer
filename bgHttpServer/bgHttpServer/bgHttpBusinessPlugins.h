#ifndef _BG_HTTP_BUSINESS_PLUGINS_H_
#define _BG_HTTP_BUSINESS_PLUGINS_H_

//////////////////////////////////////////////////////////////////////////
//
// ������Ϊ����http����ҵ��ģ�͵������
//
//////////////////////////////////////////////////////////////////////////

class bgHttpBusinessPlugins
{
public:
	/**
	 * �ж��Ƿ�Ϊ�Լ����ĵ���Ϣ����HTTPͷ�������֮��Ϳ����ж�
	 * ����ǣ������и������ݣ�������ʵ�������ܳ���
	 */
	virtual bool IsMyMsg(const char *path) = 0;

	/**
	 * ����HTTPʵ�������ܳ��ȣ�׼������Ӧ��
	 */
	virtual int SetHttpContentLength(int data_len) = 0;

	/**
	 * ����HTTPʵ������
	 * ������
	 *	@data		ʵ������
	 *	@data_len	���ݳ���
	 */ 
	virtual int CacheHttpContentData(const unsigned char *data, int data_len) = 0;

	/**
	 * ������������
	 */
	virtual int HandleRequest(const char *path, const char *query) = 0;
};

#endif//_BG_HTTP_BUSINESS_PLUGINS_H_