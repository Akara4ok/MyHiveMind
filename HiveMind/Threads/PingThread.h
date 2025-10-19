//
// Created by vlad on 10/19/25.
//

#ifndef MYHIVEMIND_PINGTHREAD_H
#define MYHIVEMIND_PINGTHREAD_H

#include <atomic>
#include <thread>

#include "HttpClientWorker.h"
#include "SafeQueue.hpp"

class PingThread {
public:
    PingThread(std::string hiveId, std::shared_ptr<SafeQueue<HttpClientWorker::Promise>> requests);
    ~PingThread();

    void start(int pingTimeOut);
    void stop();

    int getPingFailed() const;

private:
    void run();

    std::atomic<int> mPingTimeOut{};
    std::atomic<bool> mRunning{false};
    std::thread mThread;
    int mPingFailed{};
    std::string mHiveID;
    std::shared_ptr<SafeQueue<HttpClientWorker::Promise>> mRequests;
};


#endif //MYHIVEMIND_PINGTHREAD_H
