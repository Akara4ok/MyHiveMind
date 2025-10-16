//
// Created by vlad on 10/16/25.
//

#include <catch2/catch_test_macros.hpp>

#include "SafeQueue.hpp"
#include <thread>

TEST_CASE("Lock free queue test", "[Common]") {
    SafeQueue<int> q;
    constexpr int items = 1'000'000;

    std::thread t1([&] {
        for (int i = 0; i < items; i++) {
            q.push_back(i);
        }
    });

    std::thread t2([&] {
        for (int i = 0; i < items; i++) {
            q.push_back(i);
        }
    });

    t1.join();
    t2.join();

    CHECK(q.size() == 2 * items);
}
