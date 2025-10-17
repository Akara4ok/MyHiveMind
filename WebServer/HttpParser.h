//
// Created by vlad on 10/17/25.
//

#ifndef MYHIVEMIND_HTTPPARSER_H
#define MYHIVEMIND_HTTPPARSER_H

#include <string>

#include <HttpRequest.h>
#include <HttpResponse.h>

class HttpParser {
public:
    void appendData(char chunk[4096], int bytes);
    std::optional<HttpRequest> parseRequest();
    std::optional<HttpResponse> parseResponse();

private:
    bool tryParseCommon();
    bool tryParseHeaders();
    bool tryParseBody();
    nlohmann::json getBodyJson() const;
    void clear();

    std::string mBuffer;

    std::string mFirstLine;
    std::unordered_map<std::string, std::string> mHeaders;
    size_t mHeaderEnd{};
    std::string mBodyRaw;
};


#endif //MYHIVEMIND_HTTPPARSER_H