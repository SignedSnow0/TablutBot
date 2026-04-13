#pragma once

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
    }

    void Shutdown() {
        RequestStop();

        mConditionVariable.notify_all();
        mPool.clear();
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

    void WorkerLoop(std::stop_token externalToken) {
        auto token = mStopSource.get_token();

        while (!token.stop_requested() && !externalToken.stop_requested()) {
            std::function<void(std::stop_token)> task;

            {
                std::unique_lock lock(mJobsMutex);
                mConditionVariable.wait(
                    lock, [&] { return mStopping || !mJobs.empty(); });

                if (mStopping && mJobs.empty()) {
                    return;
                }

                if (!mJobs.empty()) {
                    task = std::move(mJobs.front());
                    mJobs.pop();
                }
            }

            if (task) {
                task(token);
            }
        }
    }

    std::vector<std::jthread> mPool;
    std::queue<std::function<void(std::stop_token)>> mJobs;

    std::mutex mJobsMutex;
    std::condition_variable mConditionVariable;

    std::stop_source mStopSource;
    bool mStopping;
    std::jthread mTimerThread;
};
