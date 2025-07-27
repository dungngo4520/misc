#include "../libs/collections.h"
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

namespace {
    TEST(QueueTest, PushPopTest)
    {
        auto PushPopTest = []() -> bool {
            collections::LockFreeQueue<int> queue(10);
            if (!queue.initialize()) {
                return false;
            }

            queue.push(1);
            queue.push(2);
            queue.push(3);
            queue.push(4);
            queue.push(5);

            int value;
            if (!queue.pop(value) || value != 1)
                return false;

            if (!queue.pop(value) || value != 2)
                return false;

            if (!queue.pop(value) || value != 3)
                return false;

            if (!queue.pop(value) || value != 4)
                return false;

            if (!queue.pop(value) || value != 5)
                return false;

            return true;
        };
        EXPECT_TRUE(PushPopTest());
    }

    TEST(QueueTest, CapacityTest)
    {
        auto CapacityTest = []() -> bool {
            collections::LockFreeQueue<int> queue(5);
            if (!queue.initialize()) {
                return false;
            }
            for (int i = 0; i < 5; ++i) {
                if (!queue.push(i)) {
                    return false;  // Should succeed
                }
            }
            if (queue.push(5)) {
                return false;  // Should fail due to capacity
            }
            int value;
            for (int i = 0; i < 5; ++i) {
                if (!queue.pop(value) || value != i) {
                    return false;
                }
            }
            return true;
        };
        EXPECT_TRUE(CapacityTest());
    }

    TEST(QueueTest, ClearTest)
    {
        auto ClearTest = []() -> bool {
            collections::LockFreeQueue<int> queue(10);
            if (!queue.initialize()) {
                return false;
            }
            for (int i = 0; i < 10; ++i) {
                queue.push(i);
            }
            queue.clear_with_callback([](const int&) {});
            int value;
            return !queue.pop(value);  // Should be empty
        };
        EXPECT_TRUE(ClearTest());
    }
    TEST(QueueTest, EmptyQueueTest)
    {
        auto EmptyQueueTest = []() -> bool {
            collections::LockFreeQueue<int> queue(10);
            if (!queue.initialize()) {
                return false;
            }
            int value;
            return !queue.pop(value);  // Should be empty
        };
        EXPECT_TRUE(EmptyQueueTest());
    }

    TEST(QueueTest, ContentionTest)
    {
        auto ContentionTest = []() -> bool {
            collections::LockFreeQueue<int> queue(collections::kUnlimited);
            if (!queue.initialize()) {
                return false;
            }

            auto push_thread = [](collections::LockFreeQueue<int>& q, int count) {
                for (int i = 0; i < count; ++i) {
                    q.push(i);
                }
            };

            const auto num_threads = std::thread::hardware_concurrency();
            const auto per_thread_push_count = 1000000;
            std::vector<std::thread> threads;
            spdlog::info(
                "Starting contention test with {} threads, each pushing {} items", num_threads, per_thread_push_count
            );
            for (unsigned int i = 0; i < num_threads; ++i) {
                threads.emplace_back(push_thread, std::ref(queue), per_thread_push_count);
            }

            for (auto& t : threads) {
                t.join();
            }

            if (queue.size() != num_threads * per_thread_push_count) {
                return false;  // Size mismatch
            }

            int value;
            for (int i = 0; i < num_threads * per_thread_push_count; ++i) {
                if (!queue.pop(value)) {
                    return false;  // Should be able to pop all items
                }
            }

            return true;
        };
        EXPECT_TRUE(ContentionTest());
    }
}  // namespace
