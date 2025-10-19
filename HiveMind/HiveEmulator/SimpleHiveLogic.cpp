//
// Created by vlad on 10/18/25.
//

#include "SimpleHiveLogic.h"

bool SimpleHiveLogic::doMove(const HiveMindState &state, double latitude, double longitude) {
    mTaskQueue.push(SimpleTask{
        SimpleTask::Move,
        latitude,
        longitude,
    });
    return true;
}

bool SimpleHiveLogic::doStop() {
    mTaskQueue.push(SimpleTask{SimpleTask::Stop});
    return true;
}

bool SimpleHiveLogic::addInterference(const HiveMindState &state, Interference interference) {
    mInterferenceCount++;
    return false;
}

bool SimpleHiveLogic::removeInterference(const HiveMindState &state, std::string id) {
    mInterferenceCount--;
    return false;
}
