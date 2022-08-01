//
// Created by elmer on 7/31/2022.
//

#ifndef HHAVDEMO_HHVEOPENSL_H
#define HHAVDEMO_HHVEOPENSL_H

#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

#include "base/HHVELogUtils.h"

#include <string>

#define HHVE_DATA_LOCATOR_BUFFER_QUEUE_SIZE 2

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

void SimpleBufferQueueCallbackWrapper(SLAndroidSimpleBufferQueueItf caller, void *pContext);

static void testHHVEOpenSL(const std::string& path);

static void testHHVEOpenSLCallback(SLAndroidSimpleBufferQueueItf caller, void *pContext);

class HHVEOpenSL {
public:
    HHVEOpenSL() {};

    virtual ~HHVEOpenSL();

    int init();

    void start();

    void stop();

    void SimpleBufferQueueCallback(SLAndroidSimpleBufferQueueItf caller);

private:
    SLObjectItf mEngineObj = nullptr;
    SLEngineItf mEngineItf = nullptr;

    SLObjectItf mOutputMixObj = nullptr;
    SLEnvironmentalReverbItf mOutputMixEnvironmentalReverbItf = nullptr;

    SLObjectItf mAudioPlayerObj = nullptr;
    SLPlayItf mAudioPlayerItf = nullptr;
    SLAndroidSimpleBufferQueueItf mSimpleBufferQueueItf = nullptr;
    SLEffectSendItf mEffectSendItf = nullptr;
    SLVolumeItf mVolumeItf = nullptr;
    SLMuteSoloItf mMuteSoloItf = nullptr;
};


#endif //HHAVDEMO_HHVEOPENSL_H
