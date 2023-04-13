
#include "VideoEncoder.h"

class VideoEncoderX264 : public VideoEncoder
{
public:
    using VideoEncoder::VideoEncoder;
    VideoEncoderX264();
    ~VideoEncoderX264();

    bool Init() override;
    bool OnFrame(AVFrame* pFrame) override;
    bool OnGetPacket(AVPackect** pPacket) override;
};
