#include <iostream>

#include "HttpClient.h"
#include "SafeQueue.hpp"
#include "Task.h"
#include "HttpServer.h"

int main() {
    std::shared_ptr<SafeQueue<Task>> safeQueue = std::make_shared<SafeQueue<Task>>();

    HttpServer server(8000, safeQueue);
    server.start();

    std::thread serverThread([&server, &safeQueue]() {
        while (true) {
            auto reqOpt = safeQueue->pop_front();
            if (reqOpt) {
                HttpResponse resp;
                nlohmann::json json;
                json["data"] = "Received";
                resp.setBody(json);
                std::cout << reqOpt->arguments.dump() << std::endl;
                bool ok = HttpConnection(reqOpt->clientFd).writeResponse(resp);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    HttpClient client("http://localhost", 8000);
    std::thread clientThread([&client]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json body;
            body["commandType"] = 0;
            nlohmann::json payload;
            payload["data"] = "Hello World!";
            body["commandPayload"] = payload;
            HttpResponse response = client.post("/", body);
            std::cout << response.body.dump() << std::endl;
        }
    });


    serverThread.join();

    server.stop();

    return 0;
}