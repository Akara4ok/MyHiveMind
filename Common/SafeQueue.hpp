//
// Created by vlad on 10/16/25.
//

#ifndef MYHIVEMIND_SAFEQUEUE_HPP
#define MYHIVEMIND_SAFEQUEUE_HPP

#include <mutex>
#include <memory>
#include <atomic>
#include <optional>

template<typename T>
class SafeQueue {
public:
    struct Node {
        T data{};
        std::atomic<Node *> next = nullptr;

        Node(T val) : data(std::move(val)), next(nullptr) {
        }
    };

    SafeQueue() {
        Node *node = new Node({});
        mHead.store(node);
        mTail.store(node);
        mSize.store(0);
        mHeadToDelete.store(node);
    };

    ~SafeQueue() {
        while (pop_front()) {
        }
        Node *current = mHeadToDelete.load();
        while (current) {
            Node *next = current->next;
            delete current;
            current = next;
        }
    }

    void push_back(T value) {
        Node *newNode = new Node(std::move(value));
        while (true) {
            Node *oldTail = mTail.load(std::memory_order_acquire);
            Node *next = oldTail->next.load(std::memory_order_acquire);

            if (oldTail != mTail.load(std::memory_order_acquire)) {
                continue;
            }

            if (next == nullptr) {
                if (oldTail->next.compare_exchange_weak(next, newNode)) {
                    mTail.compare_exchange_weak(oldTail, newNode);
                    mSize.fetch_add(1, std::memory_order_relaxed);
                    return;
                }
            } else {
                mTail.compare_exchange_weak(oldTail, next);
            }
        }
    }

    std::optional<T> pop_front() {
        while (true) {
            Node *oldHead = mHead.load(std::memory_order_acquire);
            Node *next = oldHead->next.load(std::memory_order_acquire);

            Node *oldTail = mTail.load(std::memory_order_acquire);

            if (oldHead != mHead.load(std::memory_order_acquire)) {
                continue;
            }

            if (oldHead == oldTail) {
                if (next == nullptr) {
                    return {};
                }
                mTail.compare_exchange_weak(oldTail, next);
            } else {
                if (!next) {
                    return {};
                }
                T value = std::move(next->data);
                if (mHead.compare_exchange_weak(oldHead, next)) {
                    mSize.fetch_sub(1, std::memory_order_relaxed);
                    deleteLater(oldHead);
                    return value;
                }
            }
        }
    }

    uint64_t size() const { return mSize.load(); }

private:
    void deleteLater(Node *node) {

        mNodeSinceLastDelete.fetch_add(1, std::memory_order_release);
        int deleteBufferSize = mNodeSinceLastDelete.load(std::memory_order_acquire);
        while (deleteBufferSize > mMaxDeleteBuffer) {
            Node *oldHead = mHeadToDelete.load(std::memory_order_acquire);
            Node *next = oldHead->next.load(std::memory_order_acquire);

            if (oldHead != mHeadToDelete.load(std::memory_order_acquire)) {
                continue;
            }

            if (mHeadToDelete.compare_exchange_weak(oldHead, next)) {
                delete oldHead;
                mNodeSinceLastDelete.fetch_sub(1, std::memory_order_release);
                deleteBufferSize = mNodeSinceLastDelete.load(std::memory_order_acquire);
            }
        }
    };

    std::atomic<Node *> mHead{};
    std::atomic<Node *> mTail{};
    std::atomic<Node *> mHeadToDelete{};
    std::atomic<uint64_t> mSize{};

    std::atomic<uint64_t> mNodeSinceLastDelete{};

    int mMaxDeleteBuffer = 1024;
};

#endif //MYHIVEMIND_SAFEQUEUE_HPP