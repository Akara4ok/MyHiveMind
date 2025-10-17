//
// Created by vlad on 10/17/25.
//

#ifndef MYHIVEMIND_HTTPCLIENT_H
#define MYHIVEMIND_HTTPCLIENT_H

#include <string>

#include "HttpParser.h"
#include "HttpResponse.h"

class HttpClient {
public:
    HttpClient(const std::string& host, int port, int timeoutMs = 500);
    ~HttpClient();

    HttpResponse get(const std::string& path);
    HttpResponse post(const std::string& path, const nlohmann::json& body);

private:
    bool ensureConnection();
    HttpResponse sendRequest(const std::string& method, const std::string& path,
                             std::string body);
    std::string createRawRequest(const std::string& method, const std::string& path,
                                const std::string& body) const;

    std::string mHost;
    int mPort;
    int mTimeoutMs;
    int mSock = -1;
    bool mConnected = false;
    HttpParser mParser;

};


#endif //MYHIVEMIND_HTTPCLIENT_H