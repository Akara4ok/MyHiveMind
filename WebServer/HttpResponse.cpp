//
// Created by vlad on 10/16/25.
//

#include "HttpResponse.h"

#include <utility>

HttpResponse::HttpResponse(int statusCode, std::string statusMessage)
    : statusCode(statusCode), statusMessage(std::move(statusMessage)) {
    headers["Server"] = "MyWebServer/1.0";
    headers["Connection"] = "keep-alive";
}


void HttpResponse::setBody(const nlohmann::json &jsonBody) {
    headers["Content-Type"] = "application/json; charset=utf-8";
    body = jsonBody;
}

std::string HttpResponse::serialize() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";

    std::string bodyRaw = body.dump();
    oss << "Content-Length" << ": " << std::to_string(bodyRaw.size()) << "\r\n";
    for (const auto& [key, value] : headers) {
        oss << key << ": " << value << "\r\n";
    }

    oss << "\r\n" << body;

    return oss.str();
}
