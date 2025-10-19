//
// Created by vlad on 10/18/25.
//

#include "SimpleHiveLogic.h"

#include <iostream>

bool SimpleHiveLogic::doMove(const HiveMindState &state, double latitude, double longitude) {
    mTaskQueue.push(SimpleTask{
        SimpleTask::Move,
        longitude,
        latitude,
    });
    return true;
}

bool SimpleHiveLogic::doStop() {
    mTaskQueue.push(SimpleTask{SimpleTask::Stop});
    return true;
}

bool SimpleHiveLogic::addInterference(const HiveMindState &state, Interference interference) {
    mInterferenceCount++;
    std::cout << "Interference added. Total count: " << mInterferenceCount << std::endl;
    return false;
}

bool SimpleHiveLogic::removeInterference(const HiveMindState &state, std::string id) {
    mInterferenceCount--;
    std::cout << "Interference removed. Total count: " << mInterferenceCount << std::endl;
    return false;
}
