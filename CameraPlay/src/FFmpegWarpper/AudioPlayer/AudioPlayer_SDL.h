#ifndef AUDIOPLAYERSDL_H
#define AUDIOPLAYERSDL_H

#include <thread>
#include <list>

extern "C"
{
    #include <SDL.h>
    #include <SDL_audio.h>
    #include <SDL_types.h>
    #include <SDL_name.h>
    #include <SDL_main.h>
    #include <SDL_config.h>
}

#include "FFmpegWarpper/AudioPlayer/AudioPlayer.h"

class AudioPlayer_SDL : public AudioPlayer
{
public:
    AudioPlayer_SDL();
    ~AudioPlayer_SDL();

    std::list<AudioDevice> getAudiDeviceList(); //��ȡ��Ƶ�豸�б�

private:
    ///����������SDL�����ڲ�����Ƶ�������������;
    ///SDL������Ƶ���
    SDL_AudioDeviceID mAudioID;
    bool openDevice();
    bool closeDevice();

    void sdlAudioCallBack(Uint8 *stream, int len);

protected:
    static void sdlAudioCallBackFunc(void *userdata, Uint8 *stream, int len);

};

#endif // AUDIOPLAYERSDL_H
