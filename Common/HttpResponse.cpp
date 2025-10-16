//
// Created by vlad on 10/16/25.
//

#include "HttpResponse.h"

#include <utility>

HttpResponse::HttpResponse(int statusCode, std::string statusMessage)
    : mStatusCode(statusCode), mStatusMessage(std::move(statusMessage)) {
    mHeaders["Server"] = "MyWebServer/1.0";
    mHeaders["Connection"] = "keep-alive";
}

void HttpResponse::setBody(std::string body) {
    mHeaders["Content-Type"] = "text/plain; charset=utf-8";
    mBody = std::move(body);
    mHeaders["Content-Length"] = std::to_string(body.size());
}

void HttpResponse::setBody(const nlohmann::json &jsonBody) {
    mHeaders["Content-Type"] = "application/json; charset=utf-8";
    mBody = jsonBody.dump();
    mHeaders["Content-Length"] = std::to_string(mBody.size());
}

std::string HttpResponse::serialize() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << mStatusCode << " " << mStatusMessage << "\r\n";

    for (const auto& [key, value] : mHeaders) {
        oss << key << ": " << value << "\r\n";
    }

    oss << "\r\n" << mBody;

    return oss.str();
}
