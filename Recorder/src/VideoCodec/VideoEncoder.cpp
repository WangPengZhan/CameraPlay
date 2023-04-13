extern "C" 
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>
}

#include "VideoEncoder.h"

VideoEncoder::VideoEncoder()
    : m_pCodecContext(nullptr)
    , m_nDstWidth(1920)
    , m_nDstHeight(1080)
    , m_nFrameRate(25)
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

int VideoEncoder::GetDstWidth() const
{
    return m_nDstWidth;
}

void VideoEncoder::SetDstWidth(int nWidth)
{
    m_nDstWidth = nWidth;
}

int VideoEncoder::GetDstHeight() const
{
    return m_nDstHeight;
}

void VideoEncoder::SetDstHeight(int nHeight)
{
    m_nDstHeight = nHeight;
}

int VideoEncoder::GetFrameRate() const
{
    return m_nFrameRate;
}

void VideoEncoder::SetFrameRate(int nFrameRate)
{
    m_nFrameRate = nFrameRate;
}

