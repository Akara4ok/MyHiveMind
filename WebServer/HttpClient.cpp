//
// Created by vlad on 10/17/25.
//

#include "HttpClient.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "Task.h"

HttpClient::HttpClient(const std::string &host, int port, int timeoutMs) : mHost(host), mPort(port), mTimeoutMs(timeoutMs) {
}

HttpClient::~HttpClient() {
    if (mSock >= 0) {
        close(mSock);
    }
}

HttpResponse HttpClient::get(const std::string &path) {
    return sendRequest("GET", path, "");
}

HttpResponse HttpClient::post(const std::string &path, const nlohmann::json& body) {
    return sendRequest("POST", path, body.dump());
}

bool HttpClient::ensureConnection() {
    if (mConnected) {
        return true;
    }
    mSock = socket(AF_INET, SOCK_STREAM, 0);
    if (mSock < 0) {
        std::cerr << "Failed to create sock" << std::endl;
        return false;
    }

    timeval tv{};
    tv.tv_sec = mTimeoutMs / 1000;
    tv.tv_usec = (mTimeoutMs % 500) * 500;
    setsockopt(mSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(mSock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(mPort);
    inet_pton(AF_INET, mHost.c_str(), &serv_addr.sin_addr);

    if (connect(mSock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Failed to connect to " << mHost << ":" << mPort << std::endl;
        close(mSock);
        mSock = -1;
        return false;
    }

    return true;
}

HttpResponse HttpClient::sendRequest(const std::string &method, const std::string &path, std::string body) {
    if (!ensureConnection()) {
        std::cerr << "Failed to connect to " << mHost << ":" << mPort << std::endl;
        return {};
    }

    std::string request = createRawRequest(method, path, body);
    ssize_t sent = send(mSock, request.c_str(), request.size(), 0);
    if (sent < 0) {
        std::cerr << "Failed to send request" << std::endl;
        return {};
    }

    char buffer[4096];
    ssize_t bytes;

    std::optional<HttpResponse> response;
    while (!response) {
        if ((bytes = recv(mSock, buffer, sizeof(buffer), 0)) <= 0) {
            std::cerr << "Failed to receive request" << std::endl;
            return {};
        }
        mParser.appendData(buffer, bytes);
        response = mParser.parseResponse();
    }

    if (!response) {
        mConnected = false;
        close(mSock);
        mSock = -1;
    }

    return response.value_or(HttpResponse());
}

std::string HttpClient::createRawRequest(const std::string &method, const std::string &path, const std::string& body) const {
    std::ostringstream request;
    request << method << " " << path << " HTTP/1.1\r\n";
    request << "Host: " << mHost << "\r\n";
    request << "Connection: keep-alive\r\n";

    std::unordered_map<std::string, std::string> headers;
    if (method == "POST") {
        request << "Content-Type" << ": " << "application/json" << "\r\n";
        request << "Content-Length" << ": " << std::to_string(body.size()) << "\r\n";
    }

    request << "\r\n";
    if (!body.empty())
        request << body;

    return request.str();
}

