//
// Created by vlad on 10/16/25.
//

#ifndef MYHIVEMIND_WEBSERVER_H
#define MYHIVEMIND_WEBSERVER_H

#include <atomic>
#include <thread>
#include <memory>
#include "HttpRequest.h"
#include "HttpConnection.h"
#include "SafeQueue.hpp"

class WebServer {
public:
    WebServer(int port, std::shared_ptr<SafeQueue<HttpRequest>> queue);
    ~WebServer();

    void start();
    void stop();

private:
    void run();
    void handleNewConnection();
    void handleClientData(int clientFd);
    void closeInactiveConnections();

    int mPort{};
    int mListenFd = -1;
    int mEpollFd = -1;
    std::atomic<bool> mRunning{false};
    std::thread mThread;

    std::shared_ptr<SafeQueue<HttpRequest>> mQueue;

    std::unordered_map<int, std::unique_ptr<HttpConnection>> mConnections;
};


#endif //MYHIVEMIND_WEBSERVER_H