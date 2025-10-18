//
// Created by vlad on 10/16/25.
//

#include <catch2/catch_test_macros.hpp>

#include "HttpResponse.h"
#include "HttpServer.h"
#include "HttpClient.h"
#include <thread>
#include <iostream>

TEST_CASE("HttpResponse - Json", "[WebServer]") {
    HttpResponse resp(200, "OK");

    nlohmann::json j;
    j["message"] = "Hello!";
    j["success"] = true;
    resp.setBody(j);

    auto l = resp.serialize();

    CHECK(resp.serialize() ==
        "HTTP/1.1 200 OK\r\nContent-Length: 35\r\nContent-Type: application/json; charset=utf-8\r\nConnection: keep-alive\r\nServer: MyWebServer/1.0\r\n\r\n{\"message\":\"Hello!\",\"success\":true}")
    ;
}

TEST_CASE("WebServer - ClientServerCommunication", "[WebServer]") {
    auto safeQueue = std::make_shared<SafeQueue<HttpRequest>>();

    HttpServer server(8000, safeQueue);
    server.start();

    int n = 5;

    std::thread serverThread([safeQueue, n]() {
        int received = 0;
        while (received < n) {
            if (auto reqOpt = safeQueue->pop_front()) {
                received++;

                HttpResponse resp;
                nlohmann::json json;
                json["received"] = received;
                resp.setBody(json);

                CHECK(reqOpt->body.value("index", 0) == received - 1);
                CHECK(HttpConnection(reqOpt->clientFd).writeResponse(resp));
            }
        }
    });

    HttpClient client("http://localhost", 8000, "localhost");
    std::thread clientThread([&client, n]() {
        for (int i = 0; i < n; ++i) {
            nlohmann::json body;
            body["commandType"] = 0;
            nlohmann::json payload;
            payload["index"] = i;
            body["commandPayload"] = payload;
            auto response = client.post("/", body);
            REQUIRE(response);
            CHECK(response->statusCode == 200);
            CHECK(response->body.value("received", 0) == i + 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });

    serverThread.join();
    clientThread.join();

    server.stop();
}
