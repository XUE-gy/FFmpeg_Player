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
		SDL_Delay(40);
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
	AVPacket* packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext* img_convert_ctx;
	//输入文件路径
	char filepath[] = "J:/学习4/多媒体播放器实践/工具/testvideo/Titanic.ts";
	int frame_cnt;
	const int bpp = 12;


	const int pixel_w = 640, pixel_h = 360;

	//unsigned char buffer[pixel_w * pixel_h * bpp / 8];



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
	if (SDL_Init(SDL_INIT_VIDEO)) {
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
	wanted_spec.freq = pCodecCtx2->sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = pCodecCtx2->channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = 1024;//音频缓冲区大小
	wanted_spec.callback = audio_callback;
	wanted_spec.userdata = pCodecCtx2;
	if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
		printf("can't open audio.\n");
		return -1;
	}
	SDL_PauseAudio(0);

	int pcm_buffer_size = 1024;
	char* pcm_buffer = (char*)malloc(pcm_buffer_size);
	int data_count = 0;


	frame_cnt = 0;
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoindex) {

			//SDL_Rect sdlRect;
			//SDL_Event event;


			//ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			//if (ret < 0) {
			//	printf("Decode Error.\n");
			//	return -1;
			//}
			//if (got_picture) {
			//	sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			//		pFrameYUV->data, pFrameYUV->linesize);
			//	printf("Decoded frame index: %d\n", frame_cnt); frame_cnt++;

			//	//SDL_Delay(40);

			//}
			//SDL_WaitEvent(&event);
			//if (event.type == REFRESH_EVENT) {
			//	SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
			//	sdlRect.x = 0;
			//	sdlRect.y = 0;
			//	sdlRect.w = screen_w;
			//	sdlRect.h = screen_h;
			//	SDL_RenderClear(sdlRenderer);
			//	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
			//	SDL_RenderPresent(sdlRenderer);

			//}
			//else if (event.type == SDL_WINDOWEVENT) {//System own function
			//   //If Resize
			//	SDL_GetWindowSize(screen, &screen_w, &screen_h);
			//}
			//else if (event.type == SDL_QUIT) {
			//	thread_exit = 1;
			//}
			//else if (event.type == BREAK_EVENT) {
			//	break;
			//}

		}
		else if (packet->stream_index == audioindex) {
			printf("音频帧");
			data_count += pcm_buffer_size;
			//Set audio buffer (PCM data)
			pcm_buffer =

			audio_chunk = (Uint8*)pcm_buffer;
			//Audio buffer length
			audio_len = pcm_buffer_size;
			audio_pos = audio_chunk;
			while (audio_len > 0)//Wait until finish
				SDL_Delay(1);

		}
		av_free_packet(packet);
	}


	SDL_Quit();

	free(pcm_buffer);

	sws_freeContext(img_convert_ctx);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);


	return 0;
}
