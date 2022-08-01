//
// Created by elmer on 8/1/2022.
//

#ifndef HHAVDEMO_HHVEDECODER_H
#define HHAVDEMO_HHVEDECODER_H

#include <string>
#include <unordered_map>

#include "base/HHVEFrameBase.h"
#include "base/HHVESafeQueue.h"

namespace HHVEDecode {
    enum HHVEDecodeType {
        HHVEDecodeTypeUnknown,
        HHVEDecodeTypeVideo,
        HHVEDecodeTypeAudio
    };

    enum HHVEValueType {
        HHVEValueTypeString,
        HHVEValueTypeFloat,
        HHVEValueTypeDouble,
        HHVEValueTypeInt,
        HHVEValueTypeLong
    };
}

class HHVEDecoder {
public:
    HHVEDecoder(HHVEDecode::HHVEDecodeType type);

    virtual ~HHVEDecoder();

    void setParameters(const std::string& key, const std::string& value, HHVEDecode::HHVEValueType type);

    int init();

    void start();

    void pause();

private:
    bool checkParameterVaild(const std::string& key, HHVEDecode::HHVEValueType type)

private:
    HHVEDecode::HHVEDecodeType mDecodeType = HHVEDecode::HHVEDecodeTypeUnknown;

    std::unordered_map<std::string, std::pair<HHVEDecode::HHVEValueType, std::string>> mParameters;

    std::shared_ptr<HHVESafeQueue<HHVEFrameBase>> mFrameQueue;

    AVFormatContext* mAVFormatContext = nullptr;
    AVCodecContext* mAVCodecContext = nullptr;
    int64_t mStreamIndex = -1;
};


#endif //HHAVDEMO_HHVEDECODER_H
