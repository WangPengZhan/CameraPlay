#ifndef AUDIOPLAYEREVENTHANDLE_H
#define AUDIOPLAYEREVENTHANDLE_H

#include <stdint.h>
#include <list>

#include "FFmpegWarpper/AudioFrame/PCMFrame.h"

class AudioPlayerCallBack
{
public:
    virtual ~AudioPlayerCallBack();

    ///��ȡ����Ƶ����ͼ����
    virtual void onGetPcmWaveValues(const std::list<float> &leftChannelValues, const std::list<float> &rightChannelValues) = 0;

    ///��ǰ���ŵ�һ֡��Ƶ����
    virtual void onGetPcmFrame(PCMFramePtr pcmFramePtr) = 0;

    /**
     * @brief onUpdatePlayingValue �������ڲ��ŵ���Ƶǿ������
     * @param leftChannel    ��ǰ֡���������в�����DBֵƽ��ֵ���Ŵ�5���ˣ�
     * @param rightChannel   ��ǰ֡���������в�����DBֵƽ��ֵ���Ŵ�5���ˣ�
     * @param leftChannelDbValues  ��ǰ֡���������в�����DBֵ
     * @param rightChannelDbValues ��ǰ֡���������в�����DBֵƽ��ֵ
     * @param progress      ���Ž���(0.0~1.0)
     */
    virtual void onUpdatePlayingValue(const float &leftChannel,
                                      const float &rightChannel,
                                      const std::list<float> &leftChannelDbValues,
                                      const std::list<float> &rightChannelDbValues,
                                      const float &progress) = 0;

    ///���²���ʱ��
    virtual void onUpdatePlayingTime(const uint32_t &totalTime, const uint32_t &currentTime) = 0;

};

#endif // AUDIOPLAYEREVENTHANDLE_H
