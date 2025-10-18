//
// Created by vlad on 10/18/25.
//

#include "HiveEmulator.h"

#include <iostream>
#include <utility>

HiveEmulator::HiveEmulator(std::unique_ptr<IHiveLogic> logic) : mLogic(std::move(logic)) {
}

HiveEmulator::~HiveEmulator() {
    stop();
}

void HiveEmulator::start() {
    if (mRunning) {
        return;
    }
    mRunning = true;
    mLogicThread = std::thread(&HiveEmulator::run, this);
}

void HiveEmulator::stop() {
    mRunning = false;
    if (mLogicThread.joinable()) {
        mLogicThread.join();
    }
}

void HiveEmulator::doMove(double longitude, double latitude) {
    if (!isValid()) {
        std::cerr << "State is not valid";
        return;
    }

    std::unique_lock lock(mMutex);
    mAbortPrevTasks = mLogic->doMove(mState, latitude, longitude);
    mCond.notify_one();
}

void HiveEmulator::doStop() {
    if (!isValid()) {
        std::cerr << "State is not valid";
        return;
    }

    std::unique_lock lock(mMutex);
    mAbortPrevTasks = mLogic->doStop();
    mCond.notify_one();
}

void HiveEmulator::addInterference(Interference interference) {
    if (!isValid()) {
        std::cerr << "State is not valid";
        return;
    }

    std::unique_lock lock(mMutex);
    mAbortPrevTasks = mLogic->addInterference(mState, std::move(interference));
    mCond.notify_one();
}

void HiveEmulator::removeInterference(std::string id) {
    if (!isValid()) {
        std::cerr << "State is not valid";
        return;
    }

    std::unique_lock lock(mMutex);
    mAbortPrevTasks = mLogic->removeInterference(mState, std::move(id));
    mCond.notify_one();
}

void HiveEmulator::setError(bool error) {
    std::unique_lock lock(mMutex);
    mState.state = error ? HiveMindState::Error : HiveMindState::Stop;
}

HiveMindState HiveEmulator::getHiveMindState() const {
    std::unique_lock lock(mMutex);
    return mState;
}

void HiveEmulator::setHiveMindState(HiveMindState state) {
    std::unique_lock lock(mMutex);
    mState = state;
}

bool HiveEmulator::isValid() const {
    return mState.state != HiveMindState::Error;
}

void HiveEmulator::doTask(IHiveLogic::SimpleTask task) {
}

void HiveEmulator::run() {
    while (mRunning) {
        std::unique_lock lock(mMutex);
        mCond.wait(lock, [this] { return mRunning || mLogic->taskCount() > 0; });

        if (mLogic->taskCount() == 0 || !mRunning) {
            return;
        }

        auto task = mLogic->popTask();
        if (!task) {
            std::cerr << "Failed to pop task";
            continue;
        }
        doTask(*task);
    }
}
