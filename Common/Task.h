//
// Created by vlad on 10/17/25.
//

#ifndef MYHIVEMIND_TASK_H
#define MYHIVEMIND_TASK_H

#include <nlohmann/json.hpp>

struct Task {
    enum TaskType {
        None,
        Stop,
        Move,
        GetTelemetry,
        SetState,
        AddInterference,
        RemoveInterference,
        SetError,
    };

    static TaskType intToType(int value);

    TaskType type{};
    int clientFd;
    nlohmann::json arguments;
};


#endif //MYHIVEMIND_TASK_H