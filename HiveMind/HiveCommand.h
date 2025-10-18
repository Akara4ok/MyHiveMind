//
// Created by vlad on 10/17/25.
//

#ifndef MYHIVEMIND_TASK_H
#define MYHIVEMIND_TASK_H

#include <nlohmann/json.hpp>

struct HiveCommand {
    enum Type {
        None,
        Stop,
        Move,
        GetTelemetry,
        SetState,
        AddInterference,
        RemoveInterference,
        Ping,
        Connect,
    };

    static Type intToType(int value);

    Type type{};
    int clientFd;
    nlohmann::json arguments;
};


#endif //MYHIVEMIND_TASK_H
