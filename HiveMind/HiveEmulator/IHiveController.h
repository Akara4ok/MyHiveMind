//
// Created by vlad on 10/19/25.
//

#ifndef MYHIVEMIND_IHIVECONTROLLER_H
#define MYHIVEMIND_IHIVECONTROLLER_H

#include <condition_variable>

#include "IHiveLogic.h"

class IHiveController {
public:
    IHiveController(std::unique_ptr<IHiveLogic> logic);
    virtual ~IHiveController();

    void start();
    void stop();

    void doMove(double longitude, double latitude);
    void doStop();

    void addInterference(Interference interference);
    void removeInterference(std::string id);
    void setError(bool error);

    HiveMindState getHiveMindState() const;
    void setHiveMindState(const HiveMindState &state);

protected:
    virtual void executeMove(double longitude, double latitude) = 0;
    virtual void executeStop() = 0;
    std::atomic<bool> mAbortPrevTasks = false;
    HiveMindState mState;

private:
    void doTask(IHiveLogic::SimpleTask task);
    bool isValid() const;
    void run();

    std::atomic<bool> mRunning{};
    mutable std::mutex mMutex;
    std::condition_variable mCond;
    std::thread mLogicThread;
    std::unique_ptr<IHiveLogic> mLogic;
};


#endif //MYHIVEMIND_IHIVECONTROLLER_H