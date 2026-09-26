#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>
#include <stdexcept>

class ThreadPool {
private:
    std::vector<std::thread> workers;

    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;

    bool stop;

public:
    explicit ThreadPool(
        std::size_t threadCount = std::thread::hardware_concurrency()
    );

    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename Function, typename... Args>
    auto submit(Function&& function, Args&&... args)
        -> std::future<
            std::invoke_result_t<Function, Args...>
        >;
};

template <typename Function, typename... Args>
auto ThreadPool::submit(Function&& function, Args&&... args)
    -> std::future<
        std::invoke_result_t<Function, Args...>
    >
{
    using ReturnType =
        std::invoke_result_t<Function, Args...>;

    auto task = std::make_shared<
        std::packaged_task<ReturnType()>
    >(
        std::bind(
            std::forward<Function>(function),
            std::forward<Args>(args)...
        )
    );

    std::future<ReturnType> result = task->get_future();

    {
        std::lock_guard<std::mutex> lock(queueMutex);

        if (stop) {
            throw std::runtime_error(
                "Cannot submit task to stopped ThreadPool"
            );
        }

        tasks.emplace([task]() {
            (*task)();
        });
    }

    condition.notify_one();

    return result;
}