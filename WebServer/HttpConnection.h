//
// Created by vlad on 10/16/25.
//

#ifndef MYHIVEMIND_HTTPCONNECTION_H
#define MYHIVEMIND_HTTPCONNECTION_H

#include <optional>
#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpConnection {
public:
    HttpConnection(int fd);

    HttpConnection(const HttpConnection&) = delete;
    HttpConnection& operator=(const HttpConnection&) = delete;

    std::optional<HttpRequest> readRequest();
    bool writeResponse(const HttpResponse& response) const;

    int fd() const { return mFd; }

    std::chrono::steady_clock::time_point mLastActive;
private:
    bool parseRequest(HttpRequest& outRequest);

    int mFd{};
    std::string mBuffer;
};

#endif //MYHIVEMIND_HTTPCONNECTION_H