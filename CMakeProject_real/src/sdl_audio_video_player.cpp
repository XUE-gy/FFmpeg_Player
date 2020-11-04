// CMakeProject_simple.cpp: 定义应用程序的入口点。
//
#include <iostream>
//#define __STDC_CONSTANT_MACROS


#include "ffmpeg.h"



int main_refresh_video(void* opaque) {
	thread_exit = 0;
	while (thread_exit == 0) {
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(1);
	}
	thread_exit = 0;
	//Break
	SDL_Event event;
	event.type = BREAK_EVENT;
	SDL_PushEvent(&event);
	return 0;
}

//Buffer:
//|-----------|-------------|
//chunk-------pos---len-----|
static  Uint8* audio_chunk;
static  Uint32  audio_len;
static  Uint8* audio_pos;

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

void  audio_callback(void* udata, Uint8* stream, int len) {
	//SDL 2.0
	SDL_memset(stream, 0, len);
	if (audio_len == 0)
		return;
	len = (len > audio_len ? audio_len : len);

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}



#undef main
int main(int argc, char* argv[])
{

	AVFormatContext* pFormatCtx;
	int	i, videoindex, audioindex;
	AVCodecContext* pCodecCtx,*pCodecCtx2;
	AVCodec* pCodec,*pCodec2;
	AVFrame* pFrame, * pFrameYUV,*pFrame2;
	uint8_t* out_buffer;
	uint8_t* pcm_buffer;
	AVPacket* packet;
	int y_size;
	int ret,ret2 , got_picture, got_picture2;
	struct SwsContext* img_convert_ctx;
	//文件路径
	char filepath[] = "D:/test.mp4";
	//char filepath[] = "J:/学习4/多媒体播放器实践/工具/testvideo/Forrest_Gump_IMAX.mp4";
	
	int frame_cnt,frame_cnt_2;
	const int bpp = 12;
	const int pixel_w = 640, pixel_h = 360;

	struct SwrContext* au_convert_ctx;




	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex = -1;
	audioindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	if (videoindex == -1) {
		printf("Didn't find a video stream.\n");
		return -1;
	}
	for (i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioindex = i;
			break;
		}
	if (audioindex == -1) {
		printf("Didn't find a audio stream.\n");
		return -1;
	}
	else {
		printf("类型");
		//std::cout << pFormatCtx->streams[0]->codec->codec->name << std::endl;
	}
	//std::cout << "封装格式：" << pFormatCtx->iformat->name;

	//AVFormatContext->		|AVInputFormat
	//		视频流			|AVStream[0]->AVCodecContext->AVCodec
	//		音频流			|AVStream[1]->AVCodecContext->AVCodec
	
	//视频流
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return -1;
	}
	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	out_buffer = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture*)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("------------------------ -------------------------\n");
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


	//音频流
	pCodecCtx2 = pFormatCtx->streams[audioindex]->codec;
	pCodec2 = avcodec_find_decoder(pCodecCtx2->codec_id);
	if (pCodec2 == NULL) {
		printf("Codec2 not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx2, pCodec2, NULL) < 0) {
		printf("Could not open codec2.\n");
		return -1;
	}
	pFrame2 = av_frame_alloc();




	//SDL视频初始化
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	SDL_Window* screen;
	int screen_w = pCodecCtx->width, screen_h = pCodecCtx->height;
	//SDL 2.0 Support for multiple windows
	screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!screen) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}
	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

	Uint32 pixformat = 0;
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pixformat = SDL_PIXELFORMAT_IYUV;

	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
	SDL_Thread* refresh_thread = SDL_CreateThread(main_refresh_video, NULL, NULL);




	//SDL音频初始化
	SDL_AudioSpec wanted_spec;
	int64_t in_channel_layout;

	packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	av_init_packet(packet);

	//Out Audio Param
	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	//nb_samples: AAC-1024 MP3-1152
	int out_nb_samples = pCodecCtx2->frame_size;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = 44100;
	int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer Size
	int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);

	pcm_buffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	pFrame2 = av_frame_alloc();
	//SDL------------------
	//Init
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}
	//SDL_AudioSpec
	wanted_spec.freq = out_sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = out_channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = out_nb_samples;
	wanted_spec.callback = audio_callback;
	wanted_spec.userdata = pCodecCtx2;

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
		printf("can't open audio.\n");
		return -1;
	}


	//FIX:Some Codec's Context Information is missing
	in_channel_layout = av_get_default_channel_layout(pCodecCtx2->channels);
	//Swr

	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout, out_sample_fmt, out_sample_rate,
		in_channel_layout, pCodecCtx2->sample_fmt, pCodecCtx2->sample_rate, 0, NULL);
	swr_init(au_convert_ctx);

	//Play
	SDL_PauseAudio(0);


	frame_cnt = 0;
	frame_cnt_2 = 0;

	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoindex) {

			SDL_Rect sdlRect;
			SDL_Event event;
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0) {
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture) {
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);
				printf("Decoded frame index: %d\n", frame_cnt); frame_cnt++;

			//	//SDL_Delay(40);

			}
			SDL_WaitEvent(&event);
			if (event.type == REFRESH_EVENT) {
				SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
				sdlRect.x = 0;
				sdlRect.y = 0;
				sdlRect.w = screen_w;
				sdlRect.h = screen_h;
				SDL_RenderClear(sdlRenderer);
				SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
				SDL_RenderPresent(sdlRenderer);

			}
			else if (event.type == SDL_WINDOWEVENT) {//System own function
			   //If Resize
				SDL_GetWindowSize(screen, &screen_w, &screen_h);
			}
			else if (event.type == SDL_QUIT) {
				thread_exit = 1;
			}
			else if (event.type == BREAK_EVENT) {
				break;
			}

		}
		else if (packet->stream_index == audioindex) {
			ret2 = avcodec_decode_audio4(pCodecCtx2, pFrame2, &got_picture2, packet);
			if (ret2 < 0) {
				printf("Error in decoding audio frame.\n");
				return -1;
			}
			if (got_picture2 > 0) {
				swr_convert(au_convert_ctx, &pcm_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t**)pFrame2->data, pFrame2->nb_samples);

				printf("index:%5d\t pts:%lld\t packet size:%d\n", frame_cnt_2, packet->pts, packet->size);




				frame_cnt_2++;
			}
			while (audio_len > 0)//Wait until finish
				SDL_Delay(1);

			//Set audio buffer (PCM data)
			audio_chunk = (Uint8*)pcm_buffer;
			//Audio buffer length
			audio_len = out_buffer_size;
			audio_pos = audio_chunk;


		}
		av_free_packet(packet);
	}


	SDL_Quit();

	

	sws_freeContext(img_convert_ctx);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);


	free(pcm_buffer);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);


	return 0;
}
