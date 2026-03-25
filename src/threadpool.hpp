#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool
{
public:
    ThreadPool(int n_threads = std::thread::hardware_concurrency())
    {
        for (int i = 0; i < n_threads; i++)
        {
            m_workers.emplace_back([this]()
            {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_cv.wait(lock, [this]()
                        {
                            return !m_tasks.empty() || m_stop;
                        });
                        if (m_stop && m_tasks.empty())
                            return;
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();
        for (auto& worker : m_workers)
            worker.join();
    }

    // Submit a task and get a future back to wait on if needed
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        using ReturnType = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        std::future<ReturnType> future = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_cv.notify_one();
        return future;
    }

    // Submit a batch of tasks and wait for all of them to finish
    template<typename F>
    void submit_batch(std::vector<F>& tasks)
    {
        std::vector<std::future<void>> futures;
        futures.reserve(tasks.size());
        for (auto& task : tasks)
            futures.push_back(submit(task));
        for (auto& f : futures)
            f.wait();
    }

private:
    std::vector<std::thread>          m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex                        m_mutex;
    std::condition_variable           m_cv;
    bool                              m_stop = false;
};
