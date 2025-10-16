#include <iostream>

#include "SafeQueue.hpp"
#include "WebServer.h"

int main() {
    std::shared_ptr<SafeQueue<HttpRequest>> safeQueue = std::make_shared<SafeQueue<HttpRequest>>();

    WebServer server(8080, safeQueue);
    server.start();

    while (true) {
        auto reqOpt = safeQueue->pop_front();
        if (reqOpt) {
            HttpResponse resp;
            nlohmann::json json;
            json["data"] = "Received";
            resp.setBody(json);
            bool ok = HttpConnection(reqOpt->clientFd).writeResponse(resp);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    server.stop();

    return 0;
}