#pragma once

#include "utils/Logger.h"
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool {
public:
    ThreadPool(uint32_t numThreads) : mStopping(false) {
        for (uint32_t i = 0; i < numThreads; i++) {
            mPool.emplace_back([this](std::stop_token st) { WorkerLoop(st); });
        }
    }

    ~ThreadPool() {
        if (mTimerThread.joinable()) {
            mTimerThread.request_stop();
            mTimerThread.join();
        }
        Shutdown();
    }

    template <typename F> auto Submit(F &&func) {
        using returnType = std::invoke_result_t<F, std::stop_token>;

        auto task =
            std::make_shared<std::packaged_task<returnType(std::stop_token)>>(
                std::forward<F>(func));

        std::future<returnType> future = task->get_future();

        {
            std::lock_guard lock(mJobsMutex);
            mJobs.emplace([task](std::stop_token st) { (*task)(st); });
        }

        mConditionVariable.notify_one();
        return future;
    }

    void RunFor(std::chrono::milliseconds maxDuration) {
        if (mTimerThread.joinable()) {
            mTimerThread.request_stop();
            mTimerThread.join();
        }

        std::this_thread::sleep_for(maxDuration);

        RequestStop();
        WaitUntilIdle();
        ClearQueue();
        ResetState();
    }

    void Shutdown() {
        LOG_INFO("Shutdown");

        for (auto &t : mPool) {
            t.request_stop();
        }

        mConditionVariable.notify_all();
    }

private:
    void RequestStop() {
        {
            std::lock_guard lock(mJobsMutex);
            mStopping = true;
        }
        mStopSource.request_stop();
        mConditionVariable.notify_all();
    }

    void WaitUntilIdle() {
        std::unique_lock lock(mIdleMutex);
        mIdleCv.wait(lock, [&] {
            return mActiveTasks.load(std::memory_order_relaxed) == 0;
        });
    }

    void ClearQueue() {
        std::lock_guard lock(mJobsMutex);
        std::queue<std::function<void(std::stop_token)>> empty;
        std::swap(mJobs, empty);
    }

    void ResetState() {
        std::lock_guard lock(mJobsMutex);

        mStopping = false;
        mStopSource = std::stop_source();
    }

    void WorkerLoop(std::stop_token externalToken) {
        while (!externalToken.stop_requested()) {
            std::function<void(std::stop_token)> task;

            {
                std::unique_lock lock(mJobsMutex);
                mConditionVariable.wait(lock, [&] {
                    return externalToken.stop_requested() || !mJobs.empty();
                });

                if (mStopping && mJobs.empty()) {
                    continue;
                }

                if (!mJobs.empty()) {
                    task = std::move(mJobs.front());
                    mJobs.pop();
                }
            }

            if (task) {
                mActiveTasks.fetch_add(1, std::memory_order_relaxed);

                auto token = mStopSource.get_token();
                task(token);

                mActiveTasks.fetch_sub(1, std::memory_order_relaxed);

                std::lock_guard lk(mIdleMutex);
                mIdleCv.notify_all();
            }
        }
    }

    std::vector<std::jthread> mPool;
    std::queue<std::function<void(std::stop_token)>> mJobs;

    std::mutex mJobsMutex;
    std::condition_variable mConditionVariable;

    std::atomic<int> mActiveTasks = 0;
    std::condition_variable mIdleCv;
    std::mutex mIdleMutex;

    std::stop_source mStopSource;
    bool mStopping;
    std::jthread mTimerThread;
};
