#ifndef AACFRAME_H
#define AACFRAME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory>

/*
sampling_frequency_index sampling frequeny [Hz]
0x0                           96000
0x1                           88200
0x2                           64000
0x3                           48000
0x4                           44100
0x5                           32000
0x6                           24000
0x7                           22050
0x8                           16000
0x9                           2000
0xa                           11025
0xb                           8000
0xc                           reserved
0xd                           reserved
0xe                           reserved
0xf                           reserved
*/
struct ADTS_HEADER
{
    unsigned int syncword;  //12 bslbf ͬ����The bit string ��1111 1111 1111����˵��һ��ADTS֡�Ŀ�ʼ
    unsigned int id;        //1 bslbf   MPEG ��ʾ��, ����Ϊ1
    unsigned int layer;     //2 uimsbf Indicates which layer is used. Set to ��00��
    unsigned int protection_absent;  //1 bslbf  ��ʾ�Ƿ�����У��
    unsigned int profile;            //2 uimsbf  ��ʾʹ���ĸ������AAC����01 Low Complexity(LC)--- AACLC
    unsigned int sf_index;           //4 uimsbf  ��ʾʹ�õĲ������±�
    unsigned int private_bit;        //1 bslbf
    unsigned int channel_configuration;  //3 uimsbf  ��ʾ������
    unsigned int original;               //1 bslbf
    unsigned int home;                   //1 bslbf
    /*�����Ϊ�ı�Ĳ�����ÿһ֡����ͬ*/
    unsigned int copyright_identification_bit;   //1 bslbf
    unsigned int copyright_identification_start; //1 bslbf
    unsigned int aac_frame_length;               // 13 bslbf  һ��ADTS֡�ĳ��Ȱ���ADTSͷ��raw data block
    unsigned int adts_buffer_fullness;           //11 bslbf     0x7FF ˵�������ʿɱ������

    /*no_raw_data_blocks_in_frame ��ʾADTS֡����number_of_raw_data_blocks_in_frame + 1��AACԭʼ֡.
    ����˵number_of_raw_data_blocks_in_frame == 0
    ��ʾ˵ADTS֡����һ��AAC���ݿ鲢����˵û�С�(һ��AACԭʼ֡����һ��ʱ����1024���������������)
    */
    unsigned int no_raw_data_blocks_in_frame;    //2 uimsfb
};

class AACFrame;
using AACFramePtr = std::shared_ptr<AACFrame>;

class AACFrame
{
public:
    AACFrame();
    ~AACFrame();

    void setAdtsHeader(const ADTS_HEADER &adts);
    void setFrameBuffer(const uint8_t * const buffer, const unsigned int &size);

    uint8_t *getBuffer(){return mFrameBuffer;}
    unsigned int getSize(){return  mFrameBufferSize;}

private:
    ADTS_HEADER mAdtsHeader;

    uint8_t *mFrameBuffer; //aac���ݣ�����adtsͷ��
    unsigned int mFrameBufferSize; //aac���ݳ��ȣ�����adtsͷ�Ĵ�С��

};

#endif // AACFRAME_H
