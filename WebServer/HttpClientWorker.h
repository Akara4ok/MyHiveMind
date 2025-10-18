//
// Created by vlad on 10/18/25.
//

#ifndef MYHIVEMIND_HTTPCLIENTWORKER_H
#define MYHIVEMIND_HTTPCLIENTWORKER_H

#include "SafeQueue.hpp"
#include "../HiveMind/HiveCommand.h"
#include "HttpRequest.h"
#include "HttpClient.h"

class HttpClientWorker {
public:
    struct Promise {
        HttpRequest request;
        std::function<void(const HttpResponse&)> onSuccess;
        std::function<void()> onFail;
    };

    HttpClientWorker(std::unique_ptr<HttpClient> client, std::shared_ptr<SafeQueue<Promise>> reqQueue);
    ~HttpClientWorker();

    void start();
    void stop();

private:
    void run() const;

    std::atomic<bool> mRunning{false};
    std::thread mThread;
    std::unique_ptr<HttpClient> mClient;
    std::shared_ptr<SafeQueue<Promise>> mReqQueue;
};


#endif //MYHIVEMIND_HTTPCLIENTWORKER_H
