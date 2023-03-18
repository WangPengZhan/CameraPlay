#ifndef PCMFRAME_H
#define PCMFRAME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory>

//extern "C"
//{
//    #include <libavutil/samplefmt.h>
//}

class PCMFrame;
using PCMFramePtr = std::shared_ptr<PCMFrame>;

///������̶�ʹ��AV_SAMPLE_FMT_S16 44100 ˫����
class PCMFrame
{
public:
    PCMFrame();
    ~PCMFrame();

    void setFrameBuffer(const uint8_t * const buffer, const unsigned int &size);

    uint8_t *getBuffer(){return mFrameBuffer;}
    unsigned int getSize(){return  mFrameBufferSize;}

private:
    uint8_t *mFrameBuffer; //pcm����
    unsigned int mFrameBufferSize; //pcm���ݳ���

//    enum AVSampleFormat mSampleFmt;//����Ĳ�����ʽ
//    int mSampleRate;//������
//    int mChannels; //������

};

#endif // PCMFRAME_H
