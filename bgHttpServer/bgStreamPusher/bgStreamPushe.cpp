#include "bgStreamPushe.h"

extern "C" 
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
};

bgStreamPusher::bgStreamPusher()
{

}

bgStreamPusher::~bgStreamPusher()
{

}

int bgStreamPusher::OnInit()
{
	int errCode = 0;

	av_register_all();
	avformat_network_init();

	return errCode;
}

void bgStreamPusher::OnClose()
{
	avformat_network_deinit();
}

int bgStreamPusher::StartPush(const char *push_url)
{
	int errCode = 0;

	//////////////////////////////////////////////////////////////////////////
	//
	// 创建输出上下文
	//
	//////////////////////////////////////////////////////////////////////////

	errCode = avformat_alloc_output_context2(&output_format_context, NULL, "flv", push_url);
	if (errCode < 0)
		return errCode;

	output_format = output_format_context->oformat;

	//////////////////////////////////////////////////////////////////////////
	//
	// 创建输出流以及输出上下文
	//
	//////////////////////////////////////////////////////////////////////////

	AVCodec *output_video_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!output_video_codec)
		return -1;

	AVStream *output_video_stream = avformat_new_stream(output_format_context, output_video_codec);
	if (!output_video_stream)
		return -2;

	//AVCodecContext

	return errCode;
}