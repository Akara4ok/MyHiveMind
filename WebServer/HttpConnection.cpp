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

    mBuffer.append(buf, bytes);

    size_t headerEnd = mBuffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        return std::nullopt;
    }

    HttpRequest req;
    if (!parseRequest(req)) {
        return std::nullopt;
    }

    mLastActive = std::chrono::steady_clock::now();

    req.clientFd = mFd;
    return req;

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

bool HttpConnection::parseRequest(HttpRequest &outRequest) {
    std::istringstream ss(mBuffer);
    std::string line;

    if (!std::getline(ss, line) || line.empty()) {
        return false;
    }

    if (line.back() == '\r') {
        line.pop_back();
    }

    std::istringstream reqLine(line);
    reqLine >> outRequest.method >> outRequest.path;

    // ----- 2. Headers -----
    while (std::getline(ss, line) && line != "\r") {
        if (line.back() == '\r') {
            line.pop_back();
        }

        size_t pos = line.find(':');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        value.erase(value.begin(), std::ranges::find_if(value,
                                                        [](const unsigned char c) { return !isspace(c); }));
        outRequest.headers[key] = value;
    }

    // ----- 3. Body -----
    std::string body;
    auto it = outRequest.headers.find("Content-Length");
    if (it != outRequest.headers.end()) {
        int len = std::stoi(it->second);
        size_t bodyStart = mBuffer.find("\r\n\r\n") + 4;
        if (mBuffer.size() >= bodyStart + len) {
            body = mBuffer.substr(bodyStart, len);
        }
    }

    auto ct = outRequest.headers.find("Content-Type");
    if (ct != outRequest.headers.end()) {
        std::string ctVal = ct->second;
        std::ranges::transform(ctVal, ctVal.begin(), ::tolower);

        if (ctVal.find("application/json") != std::string::npos) {
            try {
                outRequest.body = nlohmann::json::parse(body);
            } catch (const std::exception& e) {
                std::cerr << "[HttpConnection] JSON parse error: " << e.what() << std::endl;
            }
        }
    }

    return true;
}
