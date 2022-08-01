#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>

namespace qmpc::Utils
{
    template <typename T>
    class TransactionQueue
    {
        std::mutex mtx;
        std::condition_variable q_empty, q_max;
        std::queue<T> q;
        inline static size_t max_size = 5000;

    public:
        static void setMax(size_t q_size)
        {
            max_size = q_size;
        }
        T pop()
        {
            std::unique_lock lk(mtx);
            q_empty.wait(lk, [&]()
                         { return !q.empty(); });
            auto ret = q.front();
            q.pop();
            q_max.notify_all();
            return ret;
        }
        void push(const T &triple)
        {
            std::unique_lock lk(mtx);
            q_max.wait(lk, [&]()
                       { return q.size() < TransactionQueue::max_size; });
            q.push(triple);
            q_empty.notify_all();
        }
        size_t size() const
        {
            return q.size();
        }
        bool empty() noexcept
        {
            std::lock_guard lk(mtx);
            return q.empty();
        }
    };
}