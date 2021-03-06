#ifndef _BG_HTTP_BUSINESS_PLUGINS_H_
#define _BG_HTTP_BUSINESS_PLUGINS_H_

//////////////////////////////////////////////////////////////////////////
//
// 本类型为所有http服务业务模型的虚基类
//
//////////////////////////////////////////////////////////////////////////

class bgHttpBusinessPlugins
{
public:
	/**
	 * 初始化配置信息
	 */
	virtual int Init(const char *config_ini) = 0;

	/**
	 * 判断是否为自己关心的消息，在HTTP头处理完毕之后就可以判断
	 * 如果是，并且为POST消息，那么顺便创建一个缓存文件，用于接收POST请求
	 */
	virtual bool IsMyMsg(unsigned long connect_id, const char *method, const char *path) = 0;

	/**
	 * 设置HTTP实体数据总长度，准备好相应的
	 */
	virtual int SetHttpContentLength(unsigned long connect_id, unsigned long long data_len) = 0;

	/**
	 * 缓存HTTP实体数据
	 * 参数：
	 *	@data		实体数据
	 *	@data_len	数据长度
	 */ 
	virtual int CacheHttpContentData(unsigned long connect_id, const unsigned char *data, int data_len) = 0;

	/**
	 * 处理请求数据
	 * 返回值：
	 *	0 - 完成，返回
	 *	1 - 完成，需要循环
	 */
	virtual int HandleRequest(unsigned long connect_id, const char *method, const char *path, unsigned char **response_data, int *response_len, const char *query = nullptr) = 0;

	/**
	 * 清理应答数据
	 */
	virtual void CleanupResponseData(unsigned long connect_id, const char *method, unsigned char **response_data) = 0;
};

#endif//_BG_HTTP_BUSINESS_PLUGINS_H_