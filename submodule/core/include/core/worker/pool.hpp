#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>
#include <concepts>
#include <utility>
namespace zt
{
    
    template<class TASK>
    class WorkerPool
    {
    private:
        std::queue<TASK> tasks;
        std::mutex queue_lock;
        std::condition_variable cv;
        std::atomic<size_t> task_count{0};
        std::vector<std::thread> threads;
        static_assert(std::invocable<TASK>,"TASK must be callable");

        size_t maksThread{0};

        std::atomic_bool running{false};
        std::atomic_bool accepting{false};

        struct Worker
        {
            WorkerPool<TASK>& pool;

            void operator()()
            {
                while (pool.running)
                {
                    TASK current;

                    {
                        std::unique_lock lock(pool.queue_lock);

                        pool.cv.wait(lock, [&]
                        {
                            return !pool.tasks.empty() || !pool.running;
                        });

                        if (!pool.running && pool.tasks.empty())
                            break;

                        current = std::move(pool.tasks.front());
                        pool.tasks.pop();
                    }
                    try{

                        current();
                    }catch(){}
                    task_count--;
                }
            }

            Worker(WorkerPool<TASK>& p)
                : pool(p)
            {}
        };


    public:

        WorkerPool() = default;


        explicit WorkerPool(size_t threadCount)
        {
            init(threadCount);
        }


        void init(size_t threadCount)
        {
            if (running)
                return;

            maksThread = threadCount;
            running = true;
            accepting = true;

            for(size_t i = 0; i < maksThread; i++)
            {
                threads.emplace_back(Worker(*this));
            }
        }


        template<class F>
        bool push(F&& func)
        {
            static_assert(std::invocable<F>);

            if(!accepting)
                return false;

            {
                std::lock_guard lock(queue_lock);
                tasks.emplace(std::forward<F>(func));
            }
            task_count++;
            cv.notify_one();
            return true;
        }


        /*
            stop normal:
            - tidak menerima task baru
            - tunggu semua task selesai
            - matikan worker
        */
        void stop()
        {
            accepting = false;

            while(true)
            {
                {
                   
                    if(task_count.load()==0)
                        break;
                }

                std::this_thread::yield();
            }

            running = false;
            cv.notify_all();
        }


        /*
            stop dengan timeout
            return:
            true  = berhasil selesai semua task
            false = timeout
        */
        template<class Rep, class Period>
        bool stop(std::chrono::duration<Rep,Period> timeout)
        {
            accepting = false;

            auto start = std::chrono::steady_clock::now();

            while(true)
            {
                {
                   
                    if(task_count.load()==0)
                        break;
                }


                if(std::chrono::steady_clock::now() - start >= timeout)
                {
                    return false;
                }

                std::this_thread::yield();
            }

            running = false;
            cv.notify_all();

            return true;
        }


        /*
            join otomatis melakukan stop dulu
        */
        void join()
        {
            stop();

            for(auto& it : threads)
            {
                if(it.joinable())
                    it.join();
            }

            threads.clear();
        }


        template<class Rep, class Period>
        bool join(std::chrono::duration<Rep,Period> timeout)
        {
            if(!stop(timeout))
                return false;

            for(auto& it : threads)
            {
                if(it.joinable())
                    it.join();
            }

            threads.clear();

            return true;
        }


        bool isRunning() const
        {
            return running;
        }


        size_t taskCount()
        {
            std::lock_guard lock(queue_lock);
            return tasks.size();
        }


        WorkerPool(const WorkerPool&) = delete;
        WorkerPool& operator=(const WorkerPool&) = delete;

        WorkerPool(WorkerPool&&) = delete;
        WorkerPool& operator=(WorkerPool&&) = delete;


        ~WorkerPool()
        {
            join();
        }
    };
}