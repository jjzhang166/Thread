#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "thread_guard.h"

namespace tp
{
    class thread_pool
    {
    private:
        typedef std::function<void()> task_type;

    private:
        std::atomic<bool> stop_;
        std::mutex mtx_;
        std::condition_variable cond_;

        std::queue<task_type> tasks_;
        std::vector<std::thread> threads_;
        tp::thread_guard<std::thread> tg_;

    public:
        explicit thread_pool(int n = 0);

        ~thread_pool()
        {
            stop();
            cond_.notify_all();
        }

        template<class Function, class... Args>
        std::future<typename std::result_of<Function(Args...)>::type> submit(Function&&, Args&&...);

        void stop()
        {
            /*stop_ = true;*/
            stop_.store(true, std::memory_order_release);
        }
    };

    thread_pool::thread_pool(int n)
        : stop_(false)
        , tg_(threads_)
    {
        auto nthreads = n;

        if (nthreads <= 0)
        {
            nthreads = std::thread::hardware_concurrency();
            nthreads = nthreads == 0 ? 2 : nthreads;
        }

        for (int i = 0; i != nthreads; ++i)
            threads_.push_back(std::thread(
            [this]{
            //while (!stop_){
            while (!stop_.load(std::memory_order_acquire))
            {
                task_type task;
                {
                    std::unique_lock<std::mutex> ulk(this->mtx_);
                    //this->cond_.wait(ulk, [this]{return stop_ || !this->tasks_.empty(); });
                    this->cond_.wait(ulk, [this]{return stop_.load(std::memory_order_acquire) || !this->tasks_.empty(); });
                    //if (stop_) return;
                    if (stop_.load(std::memory_order_acquire)) return;
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }
                task();
            }
        }));
    }

    template<class Function, class... Args>
    std::future<typename std::result_of<Function(Args...)>::type>
        thread_pool::submit(Function&& fcn, Args&&... args)
    {
        typedef typename std::result_of<Function(Args...)>::type return_type;
        typedef std::packaged_task<return_type()> task;

        auto t = std::make_shared<task>(
            std::bind(std::forward<Function>(fcn), std::forward<Args>(args)...));

        auto ret = t->get_future();
        {
            std::lock_guard<std::mutex> lg(mtx_);
            //if (stop_) throw std::runtime_error("thread pool has stopped");
            if (stop_.load(std::memory_order_acquire))
            {
                throw std::runtime_error("thread pool has stopped");
            }

            tasks_.emplace([t]{(*t)(); });
        }
        cond_.notify_one();
        return ret;
    }
}

#endif