//
// Created by vlad on 10/18/25.
//

#ifndef MYHIVEMIND_HIVELOGIC_H
#define MYHIVEMIND_HIVELOGIC_H

#include <queue>
#include <optional>
#include "HiveMindState.h"
#include "Interference.h"

class IHiveLogic {
public:
    struct SimpleTask {
        enum Type {Stop, Move};

        Type type;
        double longitude = 0;
        double latitude = 0;
    };

    IHiveLogic() = default;
    virtual ~IHiveLogic() = default;

    std::optional<SimpleTask> popTask();
    int taskCount() const {return mTaskQueue.size();};

    virtual bool doMove(const HiveMindState& state, double latitude, double longitude) = 0;
    virtual bool doStop() = 0;
    virtual bool addInterference(const HiveMindState& state, Interference interference) = 0;
    virtual bool removeInterference(const HiveMindState& state, std::string id) = 0;

protected:
    std::queue<SimpleTask> mTaskQueue;
};

#endif //MYHIVEMIND_HIVELOGIC_H