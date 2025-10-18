//
// Created by vlad on 10/17/25.
//

#ifndef MYHIVEMIND_HIVEMIND_H
#define MYHIVEMIND_HIVEMIND_H

#include <SafeQueue.hpp>

#include "HiveCommand.h"
#include "HiveEmulator.h"
#include "HttpClient.h"
#include "HttpClientWorker.h"
#include "HttpRequest.h"
#include "HttpServer.h"

class HiveMind {
public:
    HiveMind(const std::string& configurationPath);
    ~HiveMind();

    void start();
    void stop();

private:
    void connectToCC();
    void startPingThread();;
    void run();

    std::atomic<bool> mRunning{false};
    std::atomic<bool> mDoPing{false};
    std::thread mThread;
    std::thread mPingThread;
    int mPingProcessedSinceLast = 0;

    std::shared_ptr<SafeQueue<HttpRequest>> mReceivedQueue;
    std::shared_ptr<SafeQueue<HttpClientWorker::Promise>> mSendQueue;

    std::string mHiveIp;
    int mHivePort{};
    std::string mHiveId{};

    std::string mSchema;
    std::string mApiPath;

    int mPingTimeout{};
    int mTelemetryTimeout{};

    std::unique_ptr<HttpServer> mHttpServer;
    std::unique_ptr<HttpClientWorker> mHttpClient;
    std::unique_ptr<HiveEmulator> mEmulator;
};


#endif //MYHIVEMIND_HIVEMIND_H
