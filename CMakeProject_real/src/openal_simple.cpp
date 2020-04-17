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

ALuint Source;// ���ڲ�������
ALuint Buffer;// ��������

char wavpath[] = "J:/ѧϰ4/��ý�岥����ʵ��/����/testaudio/���ɷ�.wav";
int main(int argc, char* argv[])
{
	std::cout << "openal_use";

    InitOpenAL(); // ��ʼ��openal    
    LoadData_new(); // ����WAV����
    Play(); // ����
    UnloadData(); // ж��WAV����
    ShutdownOpenAL(); // �ر�openal
}

bool InitOpenAL()
{
    ALCdevice* pDevice = alcOpenDevice(NULL);  // ��Ĭ����Ƶ�豸
    ALCcontext* pContext = alcCreateContext(pDevice, NULL);
    alcMakeContextCurrent(pContext);
    return true;
    // ע�⣺����û�н���ʧ�ܼ�⣬������ʾ��
}

bool LoadData_new()
{
    // �������.
    const ALsizei size = 8000;
    ALsizei freq = 441000;
    ALboolean loop = 1; // ѭ������
    // ʹ��һ�����Ҳ�������
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

    // ����WAV����
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

    // Դ������λ��
    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
    // Դ�������ٶ�
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
    // �������.
    const ALsizei size = 800;
    ALsizei freq = 8000;
    ALboolean loop = 1; // ѭ������
    // ʹ��һ�����Ҳ�������
    short data[800];
    alGenBuffers(1, &Buffer);
    float max = SHRT_MAX / 4;
    float rad = 0;
    for (short& e : data)
    {
        e = (short)(max * cosf(rad));
        rad += 1.f;
    }
    // ����WAV����
    alBufferData(Buffer, AL_FORMAT_MONO16, data, size, freq);
    alGenSources(1, &Source);

    // Դ������λ��
    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
    // Դ�������ٶ�
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
    // ����
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