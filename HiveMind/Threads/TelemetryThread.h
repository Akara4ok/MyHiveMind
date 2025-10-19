//
// Created by vlad on 10/19/25.
//

#ifndef MYHIVEMIND_TELEMETRYTHREAD_H
#define MYHIVEMIND_TELEMETRYTHREAD_H

#include <string>

#include "IHiveController.h"
#include "HttpClientWorker.h"
#include "SafeQueue.hpp"


class TelemetryThread {
public:
    TelemetryThread(std::string hiveId, std::string apiPath, std::shared_ptr<IHiveController> emulator,
                    std::shared_ptr<SafeQueue<HttpClientWorker::Promise>> requests);
    ~TelemetryThread();

    void start(int telemetryTimeOut);
    void stop();
    nlohmann::json createTelemetry() const;

private:
    void run();

    std::atomic<int> mTelemetryTimeOut{};
    std::atomic<bool> mRunning{false};
    std::thread mThread;
    std::string mHiveID;
    std::string mAPIPath;
    std::shared_ptr<SafeQueue<HttpClientWorker::Promise>> mRequests;
    std::shared_ptr<IHiveController> mEmulator;
};


#endif //MYHIVEMIND_TELEMETRYTHREAD_H
