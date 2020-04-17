#include <iostream>
#include <stdio.h> 
#include <math.h> 

#include "ffmpeg.h"

bool InitOpenAL();
bool LoadData();
bool LoadData_new();
void Play();
void UnloadData();
void ShutdownOpenAL(); 
#undef main

ALuint Source;// 用于播放声音
ALuint Buffer;// 声音数据

char wavpath[] = "J:/学习4/多媒体播放器实践/工具/testaudio/大丈夫.wav";
int main(int argc, char* argv[])
{
	std::cout << "openal_use";

    InitOpenAL(); // 初始化openal    
    LoadData_new(); // 载入WAV数据
    Play(); // 播放
    UnloadData(); // 卸载WAV数据
    ShutdownOpenAL(); // 关闭openal
}

bool InitOpenAL()
{
    ALCdevice* pDevice = alcOpenDevice(NULL);  // 打开默认音频设备
    ALCcontext* pContext = alcCreateContext(pDevice, NULL);
    alcMakeContextCurrent(pContext);
    return true;
    // 注意：这里没有进行失败检测，仅用于示例
}

bool LoadData_new()
{
    // 载入变量.
    const ALsizei size = 8000;
    ALsizei freq = 441000;
    ALboolean loop = 1; // 循环播放
    // 使用一段正弦波作数据
    short data[800];
    unsigned char buffer[8000];
    alGenBuffers(1, &Buffer);
    float max = SHRT_MAX / 4;
    float rad = 0;
    for (short& e : data)
    {
        e = (short)(max * cosf(rad));
        rad += 1.f;
    }

    // 载入WAV数据
    FILE* wavp = NULL;
    wavp = fopen(wavpath, "rb+");
    if (wavp == NULL) {
        printf("cannot open this file\n");
        return false;
    }
    if (fread(buffer, 1, 8000, wavp) != 8000) {
        // Loop
        fseek(wavp, 0, SEEK_SET);
        fread(buffer, 1, 8000, wavp);
    }

    alBufferData(Buffer, AL_FORMAT_MONO16, buffer, size, freq);

    //alBufferData(Buffer, AL_FORMAT_MONO16, data, size, freq);

    alGenSources(1, &Source);

    // 源声音的位置
    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
    // 源声音的速度
    ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

    alSourcei(Source, AL_BUFFER, Buffer);
    alSourcef(Source, AL_PITCH, 1.0f);
    alSourcef(Source, AL_GAIN, 1.0f);
    alSourcefv(Source, AL_POSITION, SourcePos);
    alSourcefv(Source, AL_VELOCITY, SourceVel);
    alSourcei(Source, AL_LOOPING, loop);

    return true;
}

bool LoadData()
{
    // 载入变量.
    const ALsizei size = 800;
    ALsizei freq = 8000;
    ALboolean loop = 1; // 循环播放
    // 使用一段正弦波作数据
    short data[800];
    alGenBuffers(1, &Buffer);
    float max = SHRT_MAX / 4;
    float rad = 0;
    for (short& e : data)
    {
        e = (short)(max * cosf(rad));
        rad += 1.f;
    }
    // 载入WAV数据
    alBufferData(Buffer, AL_FORMAT_MONO16, data, size, freq);
    alGenSources(1, &Source);

    // 源声音的位置
    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
    // 源声音的速度
    ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

    alSourcei(Source, AL_BUFFER, Buffer);
    alSourcef(Source, AL_PITCH, 1.0f);
    alSourcef(Source, AL_GAIN, 1.0f);
    alSourcefv(Source, AL_POSITION, SourcePos);
    alSourcefv(Source, AL_VELOCITY, SourceVel);
    alSourcei(Source, AL_LOOPING, loop);

    return true;
}
void UnloadData()
{
    alDeleteBuffers(1, &Buffer);
    alDeleteSources(1, &Source);
}

void Play()
{
    // 播放
    alSourcePlay(Source);
    printf("Press Enter To Stop Sound\n");
    getchar();
    alSourceStop(Source);
}


void ShutdownOpenAL()
{
    ALCcontext* pContext;
    ALCdevice* pDevice;

    pContext = alcGetCurrentContext();
    pDevice = alcGetContextsDevice(pContext);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(pContext);
    alcCloseDevice(pDevice);
}