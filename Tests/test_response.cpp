//
// Created by vlad on 10/16/25.
//

#include <catch2/catch_test_macros.hpp>

#include "HttpResponse.h"
#include <thread>
#include <iostream>

TEST_CASE("HttpResponse - Json", "[Common]") {
    HttpResponse resp(200, "OK");

    nlohmann::json j;
    j["message"] = "Hello!";
    j["success"] = true;
    resp.setBody(j);

    CHECK(resp.serialize() == "HTTP/1.1 200 OK\r\nContent-Length: 35\r\nContent-Type: application/json; charset=utf-8\r\nConnection: close\r\nServer: MyWebServer/1.0\r\n\r\n{\"message\":\"Hello!\",\"success\":true}");
}
