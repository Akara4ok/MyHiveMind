//
// Created by vlad on 10/17/25.
//

#ifndef MYHIVEMIND_HTTPCLIENT_H
#define MYHIVEMIND_HTTPCLIENT_H

#include <string>

#include "HttpParser.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpClient {
public:
    HttpClient(const std::string &host, int port, std::string ip, int timeoutMs = 500);
    ~HttpClient();

    std::optional<HttpResponse> sendRequest(const HttpRequest& request);
    std::optional<HttpResponse> get(const std::string &path);
    std::optional<HttpResponse> post(const std::string &path, const nlohmann::json &body);

private:
    bool ensureConnection();
    std::optional<HttpResponse> sendRequest(const std::string &method, const std::string &path,
                                            std::string body);
    std::string createRawRequest(const std::string &method, const std::string &path,
                                 const std::string &body) const;

    std::string mHost;
    std::string mIp;
    int mPort;
    int mTimeoutMs;
    int mSock = -1;
    bool mConnected = false;
    HttpParser mParser;
};

#endif //MYHIVEMIND_HTTPCLIENT_H
