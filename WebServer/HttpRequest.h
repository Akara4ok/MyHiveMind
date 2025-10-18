//
// Created by vlad on 10/16/25.
//

#ifndef MYHIVEMIND_HTTPREQUEST_H
#define MYHIVEMIND_HTTPREQUEST_H

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

struct HttpRequest {
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
    nlohmann::json body;
    int clientFd{};
};

#endif //MYHIVEMIND_HTTPREQUEST_H
