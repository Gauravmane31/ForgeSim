#include "ForgeSim/execution/ThreadPool.hpp"

ThreadPool::ThreadPool(std::size_t threadCount)
    : stop(false)
{
    if (threadCount == 0) {
        threadCount = 1;
    }

    for (std::size_t i = 0; i < threadCount; ++i) {

        workers.emplace_back([this]() {

            while (true) {

                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex);

                    condition.wait(
                        lock,
                        [this]() {
                            return stop || !tasks.empty();
                        }
                    );

                    if (stop && tasks.empty()) {
                        return;
                    }

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();

    for (std::thread& worker : workers) {

        if (worker.joinable()) {
            worker.join();
        }
    }
}