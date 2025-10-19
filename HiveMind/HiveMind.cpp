//
// Created by vlad on 10/17/25.
//

#include "HiveMind.h"
#include <fstream>

#include "HiveEmulator.h"
#include "HiveMindState.h"
#include "Interference.h"
#include "RequestProcessor.h"
#include "SimpleHiveLogic.h"
#include "nlohmann/json.hpp"

namespace {
    int failedPingToReconnect = 2;
    int connectTimeOutMs = 5000;
}

HiveMind::HiveMind(const std::string &configurationPath) {
    std::ifstream file(configurationPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file " << configurationPath << std::endl;
        return;
    }
    nlohmann::json config;
    file >> config;

    config = config["CommunicationConfiguration"];

    mSchema = config.value("RequestSchema", "http");
    std::string ccIP = config.value("CommunicationControlIP", "localhost");

    std::string host = mSchema + "://" + ccIP;
    int ccPort = config.value("CommunicationControlPort", 8080);

    mHiveIp = config.value("HiveIP", "localhost");
    mHivePort = config.value("HivePort", 5149);
    mHiveId = config.value("HiveID", "");

    mApiPath = "/" + config.value("CommunicationControlPath", "api/v1/hive");

    std::cout << "Config read:" << std::endl;
    std::cout << "CC: " << host << " " << ccPort << " " << mApiPath << std::endl;
    std::cout << "Hive: " << mHiveIp << " " << mHivePort << " " << mHiveId << std::endl;

    mReceivedQueue = std::make_shared<SafeQueue<HttpRequest>>();
    mSendQueue = std::make_shared<SafeQueue<HttpClientWorker::Promise>>();

    mHttpServer = std::make_unique<HttpServer>(mHivePort, mReceivedQueue);
    mHttpServer->start();

    auto client = std::make_unique<HttpClient>(host, ccPort, mHiveIp);
    mHttpClient = std::make_unique<HttpClientWorker>(std::move(client), mSendQueue);
    mHttpClient->start();

    auto hiveLogic = std::unique_ptr<IHiveLogic>(new SimpleHiveLogic());
    mEmulator = std::make_shared<HiveEmulator>(std::move(hiveLogic));
    mEmulator->start();

    mPingThread = std::make_unique<PingThread>(mHiveId, mSendQueue);
    mTelemetryThread = std::make_unique<TelemetryThread>(mHiveId, mApiPath, mEmulator, mSendQueue);
}

HiveMind::~HiveMind() {
    stop();
}

void HiveMind::start() {
    if (mRunning) {
        return;
    }
    mRunning = true;
    mThread = std::thread(&HiveMind::run, this);
}

void HiveMind::stop() {
    if (mRunning) {
        mRunning = false;
        if (mThread.joinable()) {
            mThread.join();
        }
    }
    if (mHttpServer) {
        mHttpServer->stop();
    }
    if (mHttpClient) {
        mHttpClient->stop();
    }
    if (mEmulator) {
        mEmulator->stop();
    }
    if (mPingThread) {
        mPingThread->stop();
    }
    if (mTelemetryThread) {
        mTelemetryThread->stop();
    }
}

void HiveMind::connectToCC() {
    mPingThread->stop();
    mTelemetryThread->stop();

    std::cout << "Connecting to CC" << std::endl;
    HttpRequest request;
    request.method = "POST";
    request.path = mApiPath + "/connect";
    request.body["HiveSchema"] = mSchema;
    request.body["HiveIP"] = mHiveIp;
    request.body["HivePort"] = mHivePort;
    request.body["HiveID"] = mHiveId;

    HttpClientWorker::Promise promise;
    promise.request = std::move(request);
    promise.onSuccess = [this](const HttpResponse &response) {
        nlohmann::json operationalArea = response.body["OperationalArea"];
        int pingTimeout = operationalArea.value("PingIntervalMs", 0);
        int telemetryTimeOut = operationalArea.value("TelemetryIntervalMs", 0);

        HiveMindState state;
        state.height = operationalArea.value("InitialHeight", 0.0);
        state.speed = operationalArea.value("Speed", 0.0);

        nlohmann::json location = operationalArea["InitialLocation"];
        state.longitude = location.value("Longitude", 0.0);
        state.latitude = location.value("Latitude", 0.0);
        state.state = HiveMindState::Stop;

        mEmulator->setHiveMindState(state);

        nlohmann::json interferences = response.body["Interferences"];
        for (auto j: interferences) {
            mEmulator->addInterference(Interference::fromJson(j));
        }

        mConnected = true;
        std::cout << "Connected" << std::endl;
        mPingThread->start(pingTimeout);
        mTelemetryThread->start(telemetryTimeOut);
    };
    promise.onFail = [this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(connectTimeOutMs));
        mConnected = false;
        connectToCC();
    };

    mSendQueue->push_back(std::move(promise));
}

void HiveMind::run() {
    connectToCC();
    while (mRunning) {
        while (const auto req = mReceivedQueue->pop_front()) {
            auto command = RequestProcessor::toCommand(*req);
            switch (command.type) {
                case HiveCommand::AddInterference: {
                    Interference interference = Interference::fromJson(command.arguments);
                    mEmulator->addInterference(interference);
                    sendTelemetry(command.clientFd);
                    break;
                }
                case HiveCommand::RemoveInterference: {
                    std::string id = command.arguments["Id"];
                    mEmulator->removeInterference(id);
                    sendTelemetry(command.clientFd);
                    break;
                }
                case HiveCommand::Stop: {
                    mEmulator->doStop();
                    sendTelemetry(command.clientFd);
                    break;
                }
                case HiveCommand::Move: {
                    double lon = command.arguments["Longitude"];
                    double lat = command.arguments["Latitude"];
                    mEmulator->doMove(lon, lat);
                    sendTelemetry(command.clientFd);
                    break;
                }
                default: {
                    std::cout << req->method << " " << req->path << " " << req->body.dump() << std::endl;
                    break;
                }
            }
        }
        if (!mConnected) {
            continue;
        }
        if (mPingThread->getPingFailed() >= failedPingToReconnect) {
            mConnected = false;
            connectToCC();
        }
    }
}

void HiveMind::sendTelemetry(int clientFd) {
    HttpResponse response;
    response.setBody(mTelemetryThread->createTelemetry());
    if (!HttpConnection(clientFd).writeResponse(response)) {
        std::cerr << "Failed to send Telemetry" << std::endl;
    }
}
