#include <QThread>
#include <QDebug>
#include <QImage>


extern "C" 
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>

}

#include "FFmpegCaputre.h"

FFmpegCaputre::FFmpegCaputre(QObject *parent)
    : QObject(parent)
    , m_bStarted(false)
    , m_bStop(false)
    , m_thread(&FFmpegCaputre::CaptureRun, this)
{
    
}

FFmpegCaputre::~FFmpegCaputre()
{
    Stop();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void FFmpegCaputre::SetFilename(const QString& strDeviceName)
{
    m_strDeviceName = strDeviceName;
}

void FFmpegCaputre::Start()
{
    m_bStarted = true;
    m_cv.notify_one();
}

void FFmpegCaputre::Stop()
{
    m_bStop = true;
    m_bStarted = true;
    m_cv.notify_all();
}

void FFmpegCaputre::CaptureRun()
{
    // 1.初始化设备
    avdevice_register_all();
    AVFormatContext* pFormatCtx = avformat_alloc_context();

    std::unique_lock<std::mutex> lk;
    m_cv.wait(lk, [&]() { return m_bStarted; });

    // 2.打开输入源
#ifdef DShow
    std::string strDeviceName = "video=" + m_strDeviceName.toUtf8().toStdString();
    const AVInputFormat* iformat = av_find_input_format("dshow");
    AVDeviceInfoList* device_list = nullptr;
    AVDictionary* options = nullptr;
    av_dict_set(&options, "list_devices", "true", 0);
    int result = avdevice_list_input_sources(iformat, nullptr, options, &device_list);
    if (result < 0)
    {
        return;
    }
    else
    {
        int nSize = device_list->nb_devices;
        for (int i = 0; i < nSize; ++i)
        {
            qDebug() << device_list->devices[i]->device_name << " " << device_list->devices[i]->device_description;
        }
    }
    
    AVDictionary* options = nullptr;
    av_dict_set(&options, "video_size", "640x480", 0);
    av_dict_set(&options, "pixel_format", "yuyv422", 0);
    av_dict_set(&options, "framerate", "30", 0);

    if (avformat_open_input(&pFormatCtx, strDeviceName.c_str(), iformat, nullptr) != 0)
    {
        qDebug() << "can't open file";
        return;
    }
#else
    std::string strDeviceName = m_strDeviceName.toUtf8().toStdString();
    if (avformat_open_input(&pFormatCtx, strDeviceName.c_str(), nullptr, nullptr) != 0)
    {
        qDebug() << "can't open file";
        return;
    }

#endif
    // 3.查找流信息
    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)
    {
        qDebug() << "Could't find stream infomation.";
        return;
    }


    // 4.读取视频流
    int videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1)
    {
        printf("Didn't find a video stream.");
        return;
    }

    // 5.查找解码器
    AVCodecParameters* pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
    const AVCodec* pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pCodecParameters);
    if (pCodec == nullptr)
    {
        printf("Codec not found.");
        return;
    }

    // 6.打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0)
    {
        printf("Could not open codec.");
        return;
    }

    AVFrame* pFrame = av_frame_alloc();
    AVFrame* pFrameRGB = av_frame_alloc();
    AVPacket* packet = (AVPacket*)malloc(sizeof(AVPacket));

    SwsContext* img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
        pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
        AV_PIX_FMT_RGB32, SWS_BICUBIC, nullptr, nullptr, nullptr);


    auto numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height,1);
    uint8_t* out_buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer, AV_PIX_FMT_RGB32,
        pCodecCtx->width, pCodecCtx->height, 1);

    while (true)
    {
        if (m_bStop)
        {
            break;
        }

        // 可以看出 av_read_frame 读取的是一帧视频，并存入一个 AVPacket 的结构中
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            break; // 这里认为视频读取完了
        }


        int sendPacket = avcodec_send_packet(pCodecCtx, packet);
        if (sendPacket != 0) 
        {
            continue;
        }
   
        int receiveFrame = avcodec_receive_frame(pCodecCtx, pFrame);
        if (receiveFrame != 0) 
        {

            continue;
        }

        sws_scale(img_convert_ctx,
            (uint8_t const* const*)pFrame->data,pFrame->linesize, 0, pCodecCtx->height,
            pFrameRGB->data,pFrameRGB->linesize);
      
        QImage tmpImg((uchar*)out_buffer, pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB32);
        captureImage(tmpImg);

        av_packet_unref(packet);
        QThread::msleep(20); // 停一停
    }

    // 9.回收数据
    av_free(out_buffer);
    av_free(pFrame);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
