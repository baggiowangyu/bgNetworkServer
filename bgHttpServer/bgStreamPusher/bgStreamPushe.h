#ifndef _BG_STREAM_PUSHER_H_
#define _BG_STREAM_PUSHER_H_

//////////////////////////////////////////////////////////////////////////
//
// ������
// ��Ŀ���ַ����RTSP/RTMP��
//
//////////////////////////////////////////////////////////////////////////

struct AVFormatContext;
struct AVOutputFormat;

class bgStreamPusher
{
public:
	bgStreamPusher();
	~bgStreamPusher();

public:
	int OnInit();
	void OnClose();

public:
	int StartPush(const char *push_url);
	void StopPush();

public:
	int InputStreamData(unsigned char *stream_data, int stream_len);

private:
	AVFormatContext *output_format_context;
	AVOutputFormat *output_format;
};

#endif//_BG_STREAM_PUSHER_H_