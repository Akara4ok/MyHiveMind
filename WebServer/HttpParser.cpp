//
// Created by vlad on 10/17/25.
//

#include "HttpParser.h"

#include <iostream>

void HttpParser::appendData(char chunk[4096], int bytes) {
    mBuffer.append(chunk, bytes);
}

std::optional<HttpRequest> HttpParser::parseRequest() {
    if (!tryParseCommon()) {
        return {};
    }

    if (mFirstLine.empty()) {
        clear();
        return HttpRequest();
    }

    HttpRequest request;
    std::istringstream reqLine(mFirstLine);
    reqLine >> request.method >> request.path;
    request.headers = mHeaders;
    request.body = getBodyJson();

    clear();
    return request;
}

std::optional<HttpResponse> HttpParser::parseResponse() {
    if (!tryParseCommon()) {
        return {};
    }

    if (mFirstLine.empty()) {
        clear();
        return HttpResponse();
    }

    HttpResponse response;
    std::istringstream statusLine(mFirstLine);
    std::string version;
    statusLine >> version >> response.statusCode;

    response.headers = mHeaders;
    response.body = getBodyJson();

    clear();
    return response;
}

bool HttpParser::tryParseCommon() {
    if (!tryParseHeaders()) {
        return false;
    }

    if (!tryParseBody()) {
        return false;
    }

    return true;
}

bool HttpParser::tryParseHeaders() {
    if (!mHeaders.empty()) {
        return true;
    }
    mHeaderEnd = mBuffer.find("\r\n\r\n");
    if (mHeaderEnd == std::string::npos) {
        return false;
    }

    std::string headerPart = mBuffer.substr(0, mHeaderEnd);
    std::istringstream ss(headerPart);
    std::string line;

    if (!std::getline(ss, line) || line.empty()) {
        return true;
    }
    if (line.back() == '\r') {
        line.pop_back();
    }
    mFirstLine = line;

    while (std::getline(ss, line) && line != "\r") {
        if (line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        size_t pos = line.find(':');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        val.erase(val.begin(), std::ranges::find_if(val, [](unsigned char c){ return !std::isspace(c); }));
        mHeaders[key] = val;
    }
    return true;
}

bool HttpParser::tryParseBody() {
    if (!mBodyRaw.empty()) {
        return true;
    }

    auto it = mHeaders.find("Content-Length");
    if (it != mHeaders.end()) {
        int len = std::stoi(it->second);
        size_t totalLen = mHeaderEnd + 4 + len;
        if (mBuffer.size() < totalLen) {
            return false;
        }

        mBodyRaw = mBuffer.substr(mHeaderEnd + 4, len);
        return true;
    }
    if (mBuffer.size() > mHeaderEnd + 4) {
        mBodyRaw = mBuffer.substr(mHeaderEnd + 4);
    }
    return true;
}

nlohmann::json HttpParser::getBodyJson() const {
    auto it = mHeaders.find("Content-Type");
    if (it == mHeaders.end() || mBodyRaw.empty()) {
        return {};
    }

    nlohmann::json body;
    std::string ct = it->second;
    std::ranges::transform(ct, ct.begin(), ::tolower);
    if (ct.find("application/json") != std::string::npos) {
        try {
            body = nlohmann::json::parse(mBodyRaw);
        } catch (const std::exception& e) {
            std::cerr << "[HttpParser] JSON parse error: " << e.what() << std::endl;
        }
    }
    return body;
}

void HttpParser::clear() {
    mBuffer.clear();
    mHeaders.clear();
    mBodyRaw.clear();
    mFirstLine.clear();
}
