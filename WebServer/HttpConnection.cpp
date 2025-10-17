//
// Created by vlad on 10/16/25.
//

#include "HttpConnection.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

HttpConnection::HttpConnection(int fd) : mFd(fd) {
    mLastActive = std::chrono::steady_clock::now();
}

std::optional<HttpRequest> HttpConnection::readRequest() {
    char buf[4096];
    ssize_t bytes = ::recv(mFd, buf, sizeof(buf), 0);

    if (bytes == 0) {
        return std::nullopt;
    } else if (bytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return std::nullopt;
        }
        std::cerr << "recv() failed: " << strerror(errno) << std::endl;
        return std::nullopt;
    }

    mLastActive = std::chrono::steady_clock::now();

    mParse.appendData(buf, bytes);
    auto reqOpt = mParse.parseRequest();
    if (reqOpt) {
        reqOpt->clientFd = mFd;
    }
    return reqOpt;

}

bool HttpConnection::writeResponse(const HttpResponse &response) const {
    std::string data = response.serialize();
    ssize_t totalSent = 0;
    const char* buf = data.c_str();
    size_t len = data.size();

    while (totalSent < (ssize_t)len) {
        ssize_t sent = ::send(mFd, buf + totalSent, len - totalSent, 0);
        if (sent <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            std::cerr << "send() failed: " << strerror(errno) << std::endl;
            return false;
        }
        totalSent += sent;
    }

    return true;
}
