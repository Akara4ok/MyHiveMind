//
// Created by vlad on 10/18/25.
//

#ifndef MYHIVEMIND_HIVEEMULATOR_H
#define MYHIVEMIND_HIVEEMULATOR_H

#include <condition_variable>
#include <mutex>

#include "HiveMindState.h"
#include "IHiveLogic.h"
#include "Interference.h"

class HiveEmulator {
public:
    HiveEmulator(std::unique_ptr<IHiveLogic> logic);
    ~HiveEmulator();

    void start();
    void stop();

    void doMove(double longitude, double latitude);
    void doStop();
    void addInterference(Interference interference);
    void removeInterference(std::string id);
    void setError(bool error);

    HiveMindState getHiveMindState() const;
    void setHiveMindState(HiveMindState state);

private:
    bool isValid() const;
    void doTask(IHiveLogic::SimpleTask task);
    void run();

    std::atomic<bool> mRunning{};
    bool mAbortPrevTasks = false;
    HiveMindState mState;
    mutable std::mutex mMutex;
    std::condition_variable mCond;
    std::thread mLogicThread;
    std::unique_ptr<IHiveLogic> mLogic;
};

#endif //MYHIVEMIND_HIVEEMULATOR_H