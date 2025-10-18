//
// Created by vlad on 10/16/25.
//

#include "HttpServer.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <utility>
#include <fcntl.h>
#include <sys/epoll.h>

namespace {
    int maxEvents = 1024;
    int timeOut = 5;
}

HttpServer::HttpServer(int port, std::shared_ptr<SafeQueue<HttpRequest>> queue) : mPort(port),
    mQueue(std::move(queue)) {
}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::start() {
    if (mRunning) {
        return;
    }
    mRunning = true;
    mThread = std::thread(&HttpServer::run, this);
}

void HttpServer::stop() {
    if (!mRunning) {
        return;
    }

    mRunning = false;
    for (const auto &fd: mConnections | std::views::keys) {
        epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, nullptr);
        close(fd);
    }
    mConnections.clear();

    if (mEpollFd >= 0) {
        close(mEpollFd);
    }
    if (mListenFd >= 0) {
        close(mListenFd);
    }
    if (mThread.joinable()) {
        mThread.join();
    }
}

void HttpServer::run() {
    mListenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (mListenFd < 0) {
        std::cerr << "socket() failed" << std::endl;
        return;
    }

    int opt = 1;
    setsockopt(mListenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(mPort);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(mListenFd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind() failed" << std::endl;
        close(mListenFd);
        return;
    }

    if (listen(mListenFd, SOMAXCONN) < 0) {
        std::cerr << "listen() failed" << std::endl;
        return;
    }
    fcntl(mListenFd, F_SETFL, O_NONBLOCK);

    //epoll
    mEpollFd = epoll_create1(0);
    if (mEpollFd < 0) {
        std::cerr << "epoll_create1() failed" << std::endl;
        return;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = mListenFd;
    epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mListenFd, &ev);

    std::vector<epoll_event> events(maxEvents);
    std::cout << "[WebServer] Listening on port " << mPort << "..." << std::endl;

    while (mRunning) {
        int n = epoll_wait(mEpollFd, events.data(), maxEvents, 1000);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "epoll_wait() failed" << std::endl;
            break;
        }

        closeInactiveConnections();

        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;

            if (fd == mListenFd) {
                handleNewConnection();
            } else if (events[i].events & EPOLLIN) {
                handleClientData(fd);
            } else if (events[i].events & (EPOLLHUP | EPOLLERR)) {
                epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, nullptr);
                mConnections.erase(fd);
                close(fd);
            }
        }
    }

    std::cout << "[WebServer] Shutting down..." << std::endl;
}

void HttpServer::handleNewConnection() {
    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(mListenFd, reinterpret_cast<sockaddr *>(&clientAddr), &clientLen);
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            std::cerr << "accept() failed" << std::endl;
            break;
        }

        fcntl(clientFd, F_SETFL, O_NONBLOCK);

        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.fd = clientFd;
        epoll_ctl(mEpollFd, EPOLL_CTL_ADD, clientFd, &ev);

        mConnections[clientFd] = std::make_unique<HttpConnection>(clientFd);
    }
}

void HttpServer::handleClientData(int clientFd) {
    if (!mQueue) {
        std::cerr << "[WebServer] No message queue." << std::endl;
        return;
    }

    auto it = mConnections.find(clientFd);
    if (it == mConnections.end()) {
        return;
    }

    auto &conn = it->second;
    auto reqOpt = conn->readRequest();

    if (reqOpt) {
        mQueue->push_back(*reqOpt);
        conn->mLastActive = std::chrono::steady_clock::now();
    }
}

void HttpServer::closeInactiveConnections() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = mConnections.begin(); it != mConnections.end();) {
        auto &conn = it->second;
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - conn->mLastActive).count();
        if (elapsed > timeOut) {
            epoll_ctl(mEpollFd, EPOLL_CTL_DEL, conn->fd(), nullptr);
            close(conn->fd());
            it = mConnections.erase(it);
        } else {
            ++it;
        }
    }
}
