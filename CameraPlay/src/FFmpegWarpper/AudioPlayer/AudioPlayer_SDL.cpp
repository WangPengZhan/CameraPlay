#include "AudioPlayer_SDL.h"

#include "PcmVolumeControl.h"

#include <stdio.h>

void AudioPlayer_SDL::sdlAudioCallBackFunc(void *userdata, Uint8 *stream, int len)
{
    AudioPlayer_SDL *player = (AudioPlayer_SDL*)userdata;
    player->playAudioBuffer(stream, len);
}

AudioPlayer_SDL::AudioPlayer_SDL()
{
    ///SDL��ʼ����Ҫ�������߳��У�������Щ���Ի������⡣
    if (SDL_Init(SDL_INIT_AUDIO))
    {
        fprintf(stderr, "Could not initialize SDL - %s. \n", SDL_GetError());
    }
}

AudioPlayer_SDL::~AudioPlayer_SDL()
{
    SDL_Quit();
}

std::list<AudioDevice> AudioPlayer_SDL::getAudiDeviceList()
{
    std::list<AudioDevice> deviceList;

    int num = SDL_GetNumAudioDevices(0);
    for (int i=0;i<num;i++)
    {
        AudioDevice device;
        device.deviceId = i;
        device.deviceName = SDL_GetAudioDeviceName(i, 0);

        deviceList.push_back(device);
    }

    return deviceList;
}

bool AudioPlayer_SDL::openDevice()
{
    ///��SDL�������ò��ŵĸ�ʽΪ:AUDIO_S16LSB ˫������44100hz
    ///����ʹ��ffmpeg��������Ƶ����Ҫ�ز����ɺ����һ���ĸ�ʽ�����򲥷Ż�������
    SDL_AudioSpec wanted_spec, spec;
    int wanted_nb_channels = 2;
    int samplerate = 44100;

    wanted_spec.channels = wanted_nb_channels;
    wanted_spec.freq = samplerate;
    wanted_spec.format = AUDIO_S16SYS; // ���庬����鿴��SDL�궨�塱����
    wanted_spec.silence = 0;            // 0ָʾ����
    wanted_spec.samples = 1024;  // �Զ���SDL��������С
    wanted_spec.callback = sdlAudioCallBackFunc;  // �ص�����
    wanted_spec.userdata = this;                  // ��������ص��������������

    int num = SDL_GetNumAudioDevices(0);
    for (int i=0;i<num;i++)
    {
        mAudioID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(i,0), false, &wanted_spec, &spec,0);
        if (mAudioID > 0)
        {
            break;
        }
    }

    /* ���ʵ��ʹ�õ����ã�������spec,��SDL_OpenAudio()��䣩 */
//    if (spec.format != AUDIO_S16SYS)
    if (mAudioID <= 0)
    {
        return false;
    }
    else
    {
        SDL_LockAudioDevice(mAudioID);
        SDL_PauseAudioDevice(mAudioID, 0);
        SDL_UnlockAudioDevice(mAudioID);
    }
fprintf(stderr, "mAudioID=%d\n\n\n\n\n\n", mAudioID);
    return true;
}

bool AudioPlayer_SDL::closeDevice()
{
    if (mAudioID > 0)
    {
        SDL_LockAudioDevice(mAudioID);
        SDL_PauseAudioDevice(mAudioID, 1);
        SDL_UnlockAudioDevice(mAudioID);

        SDL_CloseAudioDevice(mAudioID);
    }

    mAudioID = -1;

    return true;
}
