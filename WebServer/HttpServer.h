//
// Created by vlad on 10/16/25.
//

#ifndef MYHIVEMIND_HTTPSERVER_H
#define MYHIVEMIND_HTTPSERVER_H

#include <atomic>
#include <thread>
#include <memory>
#include "HttpConnection.h"
#include "SafeQueue.hpp"
#include "Task.h"

class HttpServer {
public:
    HttpServer(int port, std::shared_ptr<SafeQueue<Task>> queue);
    ~HttpServer();

    void start();
    void stop();

private:
    Task requestToTask(const HttpRequest &request);

    void run();
    void handleNewConnection();
    void handleClientData(int clientFd);
    void closeInactiveConnections();

    int mPort{};
    int mListenFd = -1;
    int mEpollFd = -1;
    std::atomic<bool> mRunning{false};
    std::thread mThread;

    std::shared_ptr<SafeQueue<Task>> mQueue;

    std::unordered_map<int, std::unique_ptr<HttpConnection>> mConnections;
};


#endif //MYHIVEMIND_HTTPSERVER_H