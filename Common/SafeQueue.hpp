//
// Created by vlad on 10/16/25.
//

#ifndef MYHIVEMIND_SAFEQUEUE_HPP
#define MYHIVEMIND_SAFEQUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>
#include <iostream>

template<typename T>
class SafeQueue {
public:
    SafeQueue() = default;

    ~SafeQueue() {
        close();
    };

    void push_back(T value) {
        {
            std::lock_guard lock(mMutex);
            if (mClosed) {
                std::cerr << "Safe queue is closed" << std::endl;
                return;
            }
            mQueue.push(value);
        }
        mCond.notify_one();
    }

    std::optional<T> pop_front() {
        std::unique_lock lock(mMutex);
        mCond.wait(lock, [this] { return mClosed || !empty(); });

        if (empty() || mClosed) {
            return {};
        }

        auto value = mQueue.front();
        mQueue.pop();
        return value;
    }

    void close() {
        {
            std::lock_guard lock(mMutex);
            mClosed = true;
        }
        mCond.notify_all();
    };

    bool empty() const { return mQueue.empty(); }
    size_t size() const { return mQueue.size(); }

private:
    std::queue<T> mQueue;
    std::mutex mMutex;
    std::condition_variable mCond;
    bool mClosed = false;
};

#endif //MYHIVEMIND_SAFEQUEUE_HPP
