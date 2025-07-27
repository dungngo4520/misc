#pragma once
#include <windows.h>

namespace collections {
    inline constexpr auto kUnlimited = -1;

    template <typename T>
    class LockFreeQueue {
        struct Node {
            T value;
            Node* next;
            explicit Node(const T& val) : value(val), next(nullptr) {}
        };

    public:
        explicit LockFreeQueue(const long long capacity) : head_(nullptr), tail_(nullptr), size_(0), capacity_(capacity)
        {
        }
        ~LockFreeQueue()
        {
            clear();
            delete head_;
        }

        LockFreeQueue(const LockFreeQueue&) = delete;
        LockFreeQueue& operator=(const LockFreeQueue&) = delete;
        LockFreeQueue(LockFreeQueue&&) = delete;
        LockFreeQueue& operator=(LockFreeQueue&&) = delete;

        bool initialize()
        {
            Node* dummy = new Node(T{});
            if (!dummy)
                return false;

            InterlockedExchangePointer(reinterpret_cast<void**>(&head_), dummy);
            InterlockedExchangePointer(reinterpret_cast<void**>(&tail_), dummy);
            return true;
        }

        bool push(const T& value)
        {
            if (InterlockedIncrement64(&size_) > capacity_ && capacity_ != kUnlimited) {
                InterlockedDecrement64(&size_);
                return false;  // Over capacity
            }

            Node* new_node = new Node(value);
            if (!new_node) {
                InterlockedDecrement64(&size_);
                return false;
            }

            Node* old_tail;
            while (true) {
                old_tail = tail_;
                Node* tail_next = old_tail->next;

                if (old_tail == tail_) {
                    if (tail_next == nullptr) {
                        if (InterlockedCompareExchangePointer(
                                reinterpret_cast<void**>(&old_tail->next), new_node, nullptr
                            ) == nullptr) {
                            break;
                        }
                    }
                    else {
                        InterlockedCompareExchangePointer(reinterpret_cast<void**>(&tail_), tail_next, old_tail);
                    }
                }
            }

            InterlockedCompareExchangePointer(reinterpret_cast<void**>(&tail_), new_node, old_tail);

            return true;
        }

        bool pop(T& value)
        {
            while (true) {
                Node* old_head = head_;
                Node* old_tail = tail_;
                Node* head_next = old_head->next;

                if (old_head == head_) {
                    if (old_head == old_tail) {
                        if (head_next == nullptr) {
                            return false;  // Empty
                        }
                        InterlockedCompareExchangePointer(reinterpret_cast<void**>(&tail_), head_next, old_tail);
                    }
                    else {
                        if (InterlockedCompareExchangePointer(reinterpret_cast<void**>(&head_), head_next, old_head) ==
                            old_head) {
                            value = head_next->value;
                            delete old_head;
                            InterlockedDecrement64(&size_);
                            return true;
                        }
                    }
                }
            }
        }

        template <typename Func>
        void clear_with_callback(Func func)
        {
            T value;
            while (pop(value)) {
                func(value);
            }
        }

        void clear()
        {
            clear_with_callback([](const T&) {});
        }

        long long size() { return InterlockedCompareExchange64(&size_, 0, 0); }
        long long capacity() const { return capacity_; }

    private:
        Node* head_;
        Node* tail_;
        long long size_;
        long long capacity_;
    };
}  // namespace collections
