//
// Created by vlad on 10/19/25.
//

#include "TelemetryThread.h"

#include <utility>

TelemetryThread::TelemetryThread(std::string hiveId, std::string apiPath, std::shared_ptr<IHiveController> emulator,
                                 std::shared_ptr<SafeQueue<HttpClientWorker::Promise>>
                                 requests) : mHiveID(std::move(hiveId)), mAPIPath(std::move(apiPath)),
                                             mRequests(std::move(requests)),
                                             mEmulator(std::move(emulator)) {
}

TelemetryThread::~TelemetryThread() {
    stop();
}

void TelemetryThread::start(int telemetryTimeOut) {
    mTelemetryTimeOut = telemetryTimeOut;
    if (mRunning) {
        return;
    }
    mRunning = true;
    mThread = std::thread(&TelemetryThread::run, this);
}

void TelemetryThread::stop() {
    mRunning = false;
    if (mThread.joinable()) {
        mThread.join();
    }
}

nlohmann::json TelemetryThread::createTelemetry() const {
    auto state = mEmulator->getHiveMindState();

    nlohmann::json body;
    body["HiveID"] = mHiveID;
    nlohmann::json location;
    location["Latitude"] = state.latitude;
    location["Longitude"] = state.longitude;
    body["Location"] = location;
    body["Speed"] = state.speed;
    body["Height"] = state.height;
    body["State"] = state.state + 1;
    return body;
}

void TelemetryThread::run() {
    int quant = mTelemetryTimeOut / 100;
    while (mRunning) {
        if (mTelemetryTimeOut <= 0) {
            return;
        }

        for (int i = 0; i < 100; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(quant));
            if (!mRunning) {
                return;
            }
        }

        HttpClientWorker::Promise promise;

        if (!mEmulator) {
            std::cerr << "TelemetryThread::run: Emulator is null" << std::endl;
            return;
        }

        HttpRequest request;
        request.method = "POST";
        request.path = mAPIPath + "/telemetry";
        request.body = createTelemetry();

        promise.request = std::move(request);
        promise.onSuccess = [](const HttpResponse &response) {
            std::cout << "Telemetry sent" << std::endl;
        };
        promise.onFail = []() {
            std::cerr << "Telemetry send failed" << std::endl;
        };

        mRequests->push_back(std::move(promise));
    }
}
