#ifndef AACREADER_H
#define AACREADER_H

#include "FFmpegWarpper/AudioFrame/AACFrame.h"

class AACReader
{
public:
    AACReader();

    int inputAACData(uint8_t *buf, int len); //����aac����
    void clearAACData();

    ///��AAC�����в��ҳ�һ֡����
    AACFramePtr getNextFrame();

private:
    uint8_t *mAACBuffer;
    int mBufferSize;

    ///��ȡadtsͷ
    bool ReadAdtsHeader(unsigned char * adts_headerbuf, ADTS_HEADER *adts);

};

#endif // AACREADER_H
