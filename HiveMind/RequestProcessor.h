//
// Created by vlad on 10/18/25.
//

#ifndef MYHIVEMIND_REQUESTPROCESSOR_H
#define MYHIVEMIND_REQUESTPROCESSOR_H
#include "HiveCommand.h"
#include "HttpRequest.h"

class RequestProcessor {
public:
    static HiveCommand toCommand(const HttpRequest& req);

private:
    static std::optional<HiveCommand> tryCreateAddInterference(const HttpRequest& req);
    static std::optional<HiveCommand> tryCreateRemoveInterference(const HttpRequest& req);
    static std::optional<HiveCommand> tryCreateStop(const HttpRequest& req);
    static std::optional<HiveCommand> tryCreateMove(const HttpRequest& req);

    static bool isPost(const HttpRequest& req);
    static bool isCommand(const HttpRequest& req);
};

#endif //MYHIVEMIND_REQUESTPROCESSOR_H