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
	 * ��ʼ��������Ϣ
	 */
	virtual int Init(const char *config_ini) = 0;

	/**
	 * �ж��Ƿ�Ϊ�Լ����ĵ���Ϣ����HTTPͷ�������֮��Ϳ����ж�
	 * ����ǣ�����ΪPOST��Ϣ����ô˳�㴴��һ�������ļ������ڽ���POST����
	 */
	virtual bool IsMyMsg(unsigned long connect_id, const char *method, const char *path) = 0;

	/**
	 * ����HTTPʵ�������ܳ��ȣ�׼������Ӧ��
	 */
	virtual int SetHttpContentLength(unsigned long connect_id, unsigned long long data_len) = 0;

	/**
	 * ����HTTPʵ������
	 * ������
	 *	@data		ʵ������
	 *	@data_len	���ݳ���
	 */ 
	virtual int CacheHttpContentData(unsigned long connect_id, const unsigned char *data, int data_len) = 0;

	/**
	 * ������������
	 * ����ֵ��
	 *	0 - ��ɣ�����
	 *	1 - ��ɣ���Ҫѭ��
	 */
	virtual int HandleRequest(unsigned long connect_id, const char *method, const char *path, unsigned char **response_data, int *response_len, const char *query = nullptr) = 0;

	/**
	 * ����Ӧ������
	 */
	virtual void CleanupResponseData(unsigned long connect_id, const char *method, unsigned char **response_data) = 0;
};

#endif//_BG_HTTP_BUSINESS_PLUGINS_H_