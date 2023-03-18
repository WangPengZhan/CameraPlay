#include "AudioPlayer.h"

#include "PcmVolumeControl.h"

#include <stdio.h>

AudioPlayer::AudioPlayer()
{
    mCond = new Cond();
}

AudioPlayer::~AudioPlayer()
{

}

bool AudioPlayer::startPlay()
{
    bool isSucceed = openDevice();

    return isSucceed;
}

bool AudioPlayer::stopPlay()
{
    bool isSucceed = closeDevice();

    return isSucceed;
}

void AudioPlayer::inputPCMFrame(PCMFramePtr framePtr)
{
    mCond->Lock();
    mPcmFrameList.push_back(framePtr);
    mCond->Unlock();
    mCond->Signal();
}

int AudioPlayer::getPcmFrameSize()
{
//    mCond->Lock();
    int size = mPcmFrameList.size();
//    mCond->Unlock();

    return size;
}

void AudioPlayer::playAudioBuffer(void *stream, int len)
{
    PCMFramePtr pcmFramePtr = nullptr;
//fprintf(stderr, "%s %d %d \n", __FUNCTION__, len, mPcmFrameList.size());
    mCond->Lock();
    if (!mPcmFrameList.empty())
    {
        pcmFramePtr = mPcmFrameList.front();
        mPcmFrameList.pop_front();
    }
    mCond->Unlock();

    if (pcmFramePtr != nullptr)
    {
        /// �����ҵ�PCM���ݶ���AV_SAMPLE_FMT_S16 44100 ˫�����ģ��Ҳ�������1024�����pcmFramePtr->getSize()��lenֵ��ȶ�Ϊ4096.
        /// ��������ֱ�ӿ�����ȥ�ͺ��ˡ�
//        fprintf(stderr, "%s %d %d \n", __FUNCTION__, pcmFramePtr->getSize(), len);
        memcpy(stream, (uint8_t *)pcmFramePtr->getBuffer(), pcmFramePtr->getSize());
        len -= pcmFramePtr->getSize();
    }

}
