//
// Created by vlad on 10/17/25.
//

#ifndef MYHIVEMIND_HIVEMIND_H
#define MYHIVEMIND_HIVEMIND_H

#include <SafeQueue.hpp>

#include "IHiveController.h"
#include "HttpClient.h"
#include "HttpClientWorker.h"
#include "HttpRequest.h"
#include "HttpServer.h"
#include "Threads/PingThread.h"
#include "Threads/TelemetryThread.h"

class HiveMind {
public:
    HiveMind(const std::string& configurationPath);
    ~HiveMind();

    void start();
    void stop();

private:
    void connectToCC();
    void run();
    void sendTelemetry(int clientFd);

    std::atomic<bool> mRunning{false};
    bool mConnected{false};

    std::thread mThread;

    std::shared_ptr<SafeQueue<HttpRequest>> mReceivedQueue;
    std::shared_ptr<SafeQueue<HttpClientWorker::Promise>> mSendQueue;

    std::string mHiveIp;
    int mHivePort{};
    std::string mHiveId{};

    std::string mSchema;
    std::string mApiPath;

    std::unique_ptr<HttpServer> mHttpServer;
    std::unique_ptr<HttpClientWorker> mHttpClient;
    std::shared_ptr<IHiveController> mEmulator;
    std::unique_ptr<PingThread> mPingThread;
    std::unique_ptr<TelemetryThread> mTelemetryThread;
};


#endif //MYHIVEMIND_HIVEMIND_H
