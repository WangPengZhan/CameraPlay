extern "C" 
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>
}

#include "VideoCodec.h"

VideoEncoder::VideoEncoder()
    : m_pCodecContext(nullptr)
    , m_nDstWidth(0)
    , m_nDstHeight(0)
    , m_nFrameRate(0)
{
}

VideoEncoder::VideoEncoder(int nDstWidth, int nDstHeight, int nFrameRate)
    : m_pCodecContext(nullptr)
    , m_nDstWidth(nDstWidth)
    , m_nDstHeight(nDstHeight)
    , m_nFrameRate(nFrameRate)
{

}

VideoEncoder::~VideoEncoder()
{
}

