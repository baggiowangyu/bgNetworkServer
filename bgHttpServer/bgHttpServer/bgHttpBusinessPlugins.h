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
	 * 设置HTTP实体数据总长度
	 */
	virtual int SetHttpContentLength(int data_len) = 0;

	/**
	 * 缓存HTTP实体数据
	 * 参数：
	 *	@data		实体数据
	 *	@data_len	数据长度
	 */ 
	virtual int CacheHttpContentData(const unsigned char *data, int data_len) = 0;

	/**
	 * 处理请求数据
	 */
	virtual int HandleRequest() = 0;
};

#endif//_BG_HTTP_BUSINESS_PLUGINS_H_