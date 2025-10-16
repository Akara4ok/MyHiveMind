//
// Created by vlad on 10/16/25.
//

#ifndef MYHIVEMIND_HTTPRESPONSE_H
#define MYHIVEMIND_HTTPRESPONSE_H

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

class HttpResponse {
public:
    HttpResponse(int statusCode = 200, std::string statusMessage = "OK");
    void setBody(std::string body);
    void setBody(const nlohmann::json& jsonBody);
    std::string serialize() const;

private:
    int mStatusCode = 0;
    std::string mStatusMessage;
    std::unordered_map<std::string, std::string> mHeaders;
    std::string mBody;
};


#endif //MYHIVEMIND_HTTPRESPONSE_H