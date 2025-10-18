//
// Created by vlad on 10/18/25.
//

#include "HttpClientWorker.h"

#include <utility>

HttpClientWorker::HttpClientWorker(std::unique_ptr<HttpClient> client,
                                   std::shared_ptr<SafeQueue<Promise>> reqQueue) : mClient(std::move(client)),
    mReqQueue(std::move(reqQueue)) {
}

HttpClientWorker::~HttpClientWorker() {
    stop();
}

void HttpClientWorker::start() {
    if (mRunning) {
        return;
    }
    mRunning = true;
    mThread = std::thread(&HttpClientWorker::run, this);
}

void HttpClientWorker::stop() {
    if (!mRunning) {
        return;
    }

    mRunning = false;
    if (mThread.joinable()) {
        mThread.join();
    }
}

void HttpClientWorker::run() const {
    while (mRunning) {
        if (!mClient) {
            std::cerr << "Client is null" << std::endl;
            return;
        }
        if (auto promise = mReqQueue->pop_front()) {
            auto response = mClient->sendRequest(promise->request);
            if (response) {
                if (promise->onSuccess) {
                    promise->onSuccess(*response);
                }
            } else {
                if (promise->onFail) {
                    promise->onFail();
                }
            }
        }
    }
}
