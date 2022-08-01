//
// Created by elmer on 7/31/2022.
//

#include "HHVELooper.h"

void ThreadLoopWrapper(void* ctx) {
    ((HHVELooper*)ctx)->startLoop();
}

HHVELooper::HHVELooper(int maxTaskSize) {
    mMessageQueue = std::make_shared<HHVESafeQueue<HHVELooperMessage>>(maxTaskSize);
    mThread = new std::thread(ThreadLoopWrapper, this);
}

HHVELooper::~HHVELooper() {
    HHVELooperMessage message;
    message.exit = true;
    if (mThread && mThread->joinable()) {
        mThread->join();
    }
}

void HHVELooper::postMessage(HHVELooperMessage message) {
    mMessageQueue->push(message);
}

void HHVELooper::startLoop() {
    while (true) {
        HHVELooperMessage message = mMessageQueue->pop();
        if (message.exit) {
            break;
        }
        processMessage(message);
    }
    mMessageQueue->clear();
}




