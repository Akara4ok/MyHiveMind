//
// Created by vlad on 10/19/25.
//

#include "IHiveController.h"

#include <iostream>

IHiveController::IHiveController(std::unique_ptr<IHiveLogic> logic) : mLogic(std::move(logic)) {
}

IHiveController::~IHiveController() {
    stop();
}

void IHiveController::start() {
    if (mRunning) {
        return;
    }
    mRunning = true;
    mLogicThread = std::thread(&IHiveController::run, this);
}

void IHiveController::stop() {
    mRunning = false;
    if (mLogicThread.joinable()) {
        mLogicThread.join();
    }
}

void IHiveController::doMove(double longitude, double latitude) {
    if (!isValid()) {
        std::cerr << "State is not valid";
        return;
    }

    std::unique_lock lock(mMutex);
    mAbortPrevTasks = mLogic->doMove(mState, latitude, longitude);
    mCond.notify_one();
}

void IHiveController::doStop() {
    if (!isValid()) {
        std::cerr << "State is not valid";
        return;
    }

    std::unique_lock lock(mMutex);
    mAbortPrevTasks = mLogic->doStop();
    mCond.notify_one();
}

void IHiveController::addInterference(Interference interference) {
    if (!isValid()) {
        std::cerr << "State is not valid";
        return;
    }

    std::unique_lock lock(mMutex);
    mAbortPrevTasks = mLogic->addInterference(mState, std::move(interference));
    mCond.notify_one();
}

void IHiveController::removeInterference(std::string id) {
    if (!isValid()) {
        std::cerr << "State is not valid";
        return;
    }

    std::unique_lock lock(mMutex);
    mAbortPrevTasks = mLogic->removeInterference(mState, std::move(id));
    mCond.notify_one();
}

void IHiveController::setError(bool error) {
    std::unique_lock lock(mMutex);
    mState.state = error ? HiveMindState::Error : HiveMindState::Stop;
    mAbortPrevTasks = true;
}

HiveMindState IHiveController::getHiveMindState() const {
    std::unique_lock lock(mMutex);
    return mState;
}

void IHiveController::setHiveMindState(const HiveMindState &state) {
    std::unique_lock lock(mMutex);
    mState = state;
    mAbortPrevTasks = true;
}

void IHiveController::doTask(IHiveLogic::SimpleTask task) {
    switch (task.type) {
        case IHiveLogic::SimpleTask::Stop: executeStop();
            return;
        case IHiveLogic::SimpleTask::Move: executeMove(task.longitude, task.latitude);
            return;
        default: std::cerr << "Unknown task " << task.type << std::endl;
    }
}

bool IHiveController::isValid() const {
    return mState.state != HiveMindState::Error;
}

void IHiveController::run() {
    while (mRunning) {
        {
            std::unique_lock lock(mMutex);
            mCond.wait(lock, [this] { return !mRunning || mLogic->taskCount() > 0; });
            mAbortPrevTasks = false;
        }

        auto task = mLogic->popTask();
        if (!task) {
            continue;
        }
        doTask(*task);
    }
}
