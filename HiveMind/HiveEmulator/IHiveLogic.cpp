//
// Created by vlad on 10/18/25.
//

#include "IHiveLogic.h"

std::optional<IHiveLogic::SimpleTask> IHiveLogic::popTask() {
    if (taskCount() == 0) {
        return std::nullopt;
    }
    auto task = mTaskQueue.front();
    mTaskQueue.pop();
    return task;
}
