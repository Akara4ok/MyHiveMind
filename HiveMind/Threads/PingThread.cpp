//
// Created by vlad on 10/19/25.
//

#include "PingThread.h"

#include <utility>

PingThread::PingThread(std::string hiveId,
                       std::shared_ptr<SafeQueue<HttpClientWorker::Promise>> requests) : mHiveID(std::move(hiveId)),
    mRequests(std::move(requests)) {
}

PingThread::~PingThread() {
    stop();
}

void PingThread::start(int pingTimeOut) {
    mPingFailed = 0;
    mPingTimeOut = pingTimeOut;
    if (mRunning) {
        return;
    }
    mRunning = true;
    mThread = std::thread(&PingThread::run, this);
}

void PingThread::stop() {
    mRunning = false;
    if (mThread.joinable()) {
        mThread.join();
    }
    mPingFailed = 0;
}

int PingThread::getPingFailed() const {
    return mPingFailed;
}

void PingThread::run() {
    int quant = mPingTimeOut / 100;
    while (mRunning) {
        if (mPingTimeOut <= 0) {
            return;
        }

        for (int i = 0; i < 100; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(quant));
            if (!mRunning) {
                return;
            }
        }

        HttpClientWorker::Promise promise;

        HttpRequest request;
        request.method = "GET";
        request.path = "/ping";
        request.body["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        request.body["hiveID"] = mHiveID;
        promise.request = std::move(request);
        promise.onSuccess = [this](const HttpResponse &response) {
            mPingFailed = 0;
            std::cout << "Ping processed" << std::endl;
        };
        promise.onFail = [this]() {
            mPingFailed++;
        };

        mRequests->push_back(std::move(promise));
    }
}
