#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <thread>
#include <list>
#include <string>

#include "FFmpegWarpper/Mutex/Cond.h"
#include "FFmpegWarpper/AudioFrame/PCMFrame.h"

struct AudioDevice
{
    std::string deviceName;
    uint32_t deviceId;
};

class AudioPlayer
{
public:
    AudioPlayer();
    virtual ~AudioPlayer();

    virtual std::list<AudioDevice> getAudiDeviceList() = 0; //��ȡ��Ƶ�豸�б�

    bool startPlay();
    bool stopPlay();

    void inputPCMFrame(PCMFramePtr framePtr);
    int getPcmFrameSize();

private:
    Cond *mCond;
    std::list<PCMFramePtr> mPcmFrameList;

    virtual bool openDevice()  = 0;
    virtual bool closeDevice() = 0;

protected:
    void playAudioBuffer(void *stream, int len);

};

#endif // AUDIOPLAYER_H
