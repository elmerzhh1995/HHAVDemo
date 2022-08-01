//
// Created by elmer on 7/31/2022.
//

#ifndef HHAVDEMO_HHVESAFEQUEUE_H
#define HHAVDEMO_HHVESAFEQUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class HHVESafeQueue {
public:
    HHVESafeQueue(int64_t maxSize):mMaxQueueSize(maxSize) {}

    virtual ~HHVESafeQueue() {
        clear();
    }

    bool empty() {
        std::lock_guard<std::mutex> lockGuard(mQueueMutex);
        return mQueue.empty();
    }

    int64_t size() {
        std::lock_guard<std::mutex> lockGuard(mQueueMutex);
        return mQueue.size();
    }

    void push(T item) {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        mQueuePushCond.wait(lock, [&]() {return mQueue.size() < mMaxQueueSize; });
        mQueue.push(std::move(item));
        mQueuePopCond.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        mQueuePopCond.wait(lock, [&](){return !mQueue.empty();});
        T item = std::move(mQueue.front());
        mQueue.pop();
        mQueuePushCond.notify_one();
        return item;
    }

    void clear() {
        while (!mQueue.empty()) {
            std::lock_guard<std::mutex> lockGuard(mQueueMutex);
            mQueue.pop();
            mQueuePushCond.notify_one();
        }
    }


private:
    std::queue<T> mQueue;
    std::mutex mQueueMutex;
    std::condition_variable mQueuePushCond;
    std::condition_variable mQueuePopCond;
    int64_t mMaxQueueSize;
};


#endif //HHAVDEMO_HHVESAFEQUEUE_H

