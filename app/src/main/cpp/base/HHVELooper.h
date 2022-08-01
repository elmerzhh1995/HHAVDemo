//
// Created by elmer on 7/31/2022.
//

#ifndef HHAVDEMO_HHVELOOPER_H
#define HHAVDEMO_HHVELOOPER_H

#include "HHVESafeQueue.h"
#include <thread>

static void ThreadLoopWrapper(void* ctx);

struct HHVELooperMessage {
    int what = -1;
    int arg1 = -1;
    int arg2 = -1;
    char* arg3 = nullptr;
    void* obj = nullptr;
    bool flush = false;
    bool exit = false;
};

class HHVELooper {
public:
    HHVELooper(int maxTaskSize = 10);

    virtual ~HHVELooper();

    void postMessage(HHVELooperMessage message);

    void startLoop();

protected:
    virtual void processMessage(HHVELooperMessage message) = 0;

private:
    std::thread *mThread = nullptr;

    std::shared_ptr<HHVESafeQueue<HHVELooperMessage>> mMessageQueue;
};


#endif //HHAVDEMO_HHVELOOPER_H
