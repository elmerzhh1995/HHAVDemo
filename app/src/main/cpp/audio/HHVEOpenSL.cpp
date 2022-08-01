//
// Created by elmer on 7/31/2022.
//

#include "HHVEOpenSL.h"
static FILE *testHHVEOpenSLFile = nullptr;
static std::shared_ptr<HHVEOpenSL> openSLObj = nullptr;
void testHHVEOpenSL(const std::string& path) {
    if (path.empty()) {
        return;
    }
    openSLObj = std::make_shared<HHVEOpenSL>();
    openSLObj->init();

    testHHVEOpenSLFile = fopen(path.c_str(), "rb");
}

void testHHVEOpenSLCallback(SLAndroidSimpleBufferQueueItf caller, void *pContext) {
    if (!openSLObj || !testHHVEOpenSLFile) {
        return;
    }

    char* buffer = (char*)malloc(1024 * 16);
    size_t realSize = fread(buffer, 8, 1024 * 2, testHHVEOpenSLFile);
    SLresult result = (*caller)->Enqueue(caller, buffer, realSize >> 1);
    free(buffer);
}

void SimpleBufferQueueCallbackWrapper(SLAndroidSimpleBufferQueueItf caller, void *pContext) {
    if (caller == nullptr) {
        return;
    }

    testHHVEOpenSLCallback(caller, pContext);
}

HHVEOpenSL::~HHVEOpenSL() {
    if(mAudioPlayerObj) {
        (*mAudioPlayerObj)->Destroy(mAudioPlayerObj);
        mAudioPlayerObj = nullptr;
        mAudioPlayerItf = nullptr;
        mSimpleBufferQueueItf = nullptr;
        mEffectSendItf = nullptr;
        mMuteSoloItf = nullptr;
        mVolumeItf = nullptr;
    }

    if (mOutputMixObj) {
        (*mOutputMixObj)->Destroy(mOutputMixObj);
        mOutputMixObj = nullptr;
        mOutputMixEnvironmentalReverbItf = nullptr;
    }

    if (mEngineObj) {
        (*mEngineObj)->Destroy(mEngineObj);
        mEngineObj = nullptr;
        mEngineItf = nullptr;
    }

}

int HHVEOpenSL::init() {
    auto errorCmd = [](int ret, const char* log) -> int {
        HHVELOGE("HHVEOpenSL errorCode:%d errorMessage%s", ret, log);
        return ret;
    };

    // 1. create engine
    SLresult result;
    result = slCreateEngine(&mEngineObj, 0, nullptr, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "slCreateEngine");}

    result = (*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "Realize");}

    result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngineItf);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "GetInterface");}

    // 2. create outputMix
    const int numInterfaces = 1;
    const SLInterfaceID ids[numInterfaces] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[numInterfaces] = {SL_BOOLEAN_FALSE};
    result = (*mEngineItf)->CreateOutputMix(mEngineItf, &mOutputMixObj, numInterfaces, ids, req);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "CreateOutputMix");}

    result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "Realize");}

    // 2.1 get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available, either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*mOutputMixObj)->GetInterface(mOutputMixObj, SL_IID_ENVIRONMENTALREVERB, &mOutputMixEnvironmentalReverbItf);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "GetInterface");}
    result = (*mOutputMixEnvironmentalReverbItf)->SetEnvironmentalReverbProperties(mOutputMixEnvironmentalReverbItf, &reverbSettings);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "SetEnvironmentalReverbProperties");}

    // 3. create audio player
    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue locatorAndroidSimpleBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, HHVE_DATA_LOCATOR_BUFFER_QUEUE_SIZE
    };
    SLDataFormat_PCM formatPcm = {
            SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_8, SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16, SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource audioSource = {&locatorAndroidSimpleBufferQueue, &formatPcm};
    // configure audio sink
    SLDataLocator_OutputMix locatorOutputMix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObj};
    SLDataSink audioSink = {&locatorOutputMix, NULL};
    // create audio player
    const int numInterfaces_player = 3;
    const SLInterfaceID ids_player[numInterfaces_player] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND/*, SL_IID_MUTESOLO*/};
    const SLboolean req_player[numInterfaces_player] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE/*, SL_BOOLEAN_TRUE*/};
    // create audio player
    result = (*mEngineItf)->CreateAudioPlayer(mEngineItf, &mAudioPlayerObj, &audioSource, &audioSink, numInterfaces_player, ids_player, req_player);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "CreateAudioPlayer");}
    result = (*mAudioPlayerObj)->Realize(mAudioPlayerObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "CreateAudioPlayer");}

    // get the play interface
    result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_PLAY, &mAudioPlayerItf);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "GetInterface");}

    // get the buffer queue interface and register callback on the buffer queue
    result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_BUFFERQUEUE, &mSimpleBufferQueueItf);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "GetInterface");}
    result = (*mSimpleBufferQueueItf)->RegisterCallback(mSimpleBufferQueueItf, SimpleBufferQueueCallbackWrapper, this);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "RegisterCallback");}

    // get the effect send interface
    result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_EFFECTSEND, &mEffectSendItf);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "GetInterface");}

    // mute/solo is not supported for sources that are known to be mono, as this is get the mute/solo interface
#if 1
    result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_MUTESOLO, &mMuteSoloItf);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "GetInterface");}
#endif

    // get the volume interface
    result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_VOLUME, &mVolumeItf);
    if (result != SL_RESULT_SUCCESS) { return errorCmd((int)result, "GetInterface");}

    start();
}

void HHVEOpenSL::SimpleBufferQueueCallback(SLAndroidSimpleBufferQueueItf caller) {
//    if (caller != mSimpleBufferQueueItf) {
//        return;
//    }
//    // for streaming playback, replace this test by logic to find and fill the next buffer
//    if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
//        SLresult result;
//        // enqueue another buffer
//        result = (*mSimpleBufferQueueItf)->Enqueue(mSimpleBufferQueueItf, nextBuffer, nextSize);
//        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
//        // which for this code example would indicate a programming error
//        if (SL_RESULT_SUCCESS != result) {
//            pthread_mutex_unlock(&audioEngineLock);
//        }
//        (void)result;
//    } else {
//        releaseResampleBuf();
//        pthread_mutex_unlock(&audioEngineLock);
//    }
}

void HHVEOpenSL::start() {
    SLuint32 playState;
    (*mAudioPlayerItf)->GetPlayState(mAudioPlayerItf, &playState);
    if (playState == SL_PLAYSTATE_PLAYING) {
        return;
    }

    (*mAudioPlayerItf)->SetPlayState(mAudioPlayerItf, SL_PLAYSTATE_PLAYING);
}

void HHVEOpenSL::stop() {
    SLuint32 playState;
    (*mAudioPlayerItf)->GetPlayState(mAudioPlayerItf, &playState);
    if (playState == SL_PLAYSTATE_PAUSED) {
        return;
    }

    (*mAudioPlayerItf)->SetPlayState(mAudioPlayerItf, SL_PLAYSTATE_PAUSED);
}


