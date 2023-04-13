#pragma once


struct ACCodecContext;
struct AVFrame;
struct AVPackect;

class VideoEncoder
{
public:
    VideoEncoder();
    VideoEncoder(int nDstWidth, int nDstHeight, int nFrameRate);
    ~VideoEncoder();


    int GetDstWidth() const;
    void SetDstWidth(int nWidth);
    int GetDstHeight() const;
    void SetDstHeight(int nHeight);
    int GetFrameRate() const;
    void SetFrameRate(int nFrameRate);

    virtual bool Init() = 0;
    virtual bool OnFrame(AVFrame* pFrame) = 0;
    virtual bool OnGetPacket(AVPackect** pPacket) = 0;

protected:
    ACCodecContext* m_pCodecContext;

private:
    int m_nDstWidth;
    int m_nDstHeight;
    int m_nFrameRate;
};

