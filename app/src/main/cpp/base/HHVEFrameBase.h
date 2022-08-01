//
// Created by elmer on 7/31/2022.
//

#ifndef HHAVDEMO_HHVEFRAMEBASE_H
#define HHAVDEMO_HHVEFRAMEBASE_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}

namespace HHVEFRAME {
    enum HHVEFrameType {
        HHVEFrameTypeUnknown,
        HHVEFrameTypeAudio,
        HHVEFrameTypeVideo
    };

    enum HHVEVideoFrameType {
        HHVEVideoFrameTypeUnknown,
        HHVEVideoFrameTypeRawYUV420P,
        HHVEVideoFrameTypeRawYUV422P,
        HHVEVideoFrameTypeTexture
    };
}

class HHVEFrameBase {
public:
    virtual ~HHVEFrameBase() {};

    virtual void initWithRawData(uint8_t* data, void* bundle) {};

    virtual void initWithAVFrame(AVFrame* avFrame) {};
protected:
    HHVEFRAME::HHVEFrameType mFrameType = HHVEFRAME::HHVEFrameTypeUnknown;
    uint8_t* mFrameData[AV_NUM_DATA_POINTERS];
    int mFrameLineSize[AV_NUM_DATA_POINTERS];
    int64_t mFramePts = -1;
};

class HHVEVideoFrame: public HHVEFrameBase {
private:
    int64_t mFrameWidth = -1;
    int64_t mFrameHeight = -1;
};

class HHVEAudioFrame: public HHVEFrameBase {

};

#endif //HHAVDEMO_HHVEFRAMEBASE_H
