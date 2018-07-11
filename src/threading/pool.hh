/**
 * @file    pool.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *          Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 * @brief   Class implementing a thread pool.
 * @bugs    Currently no bugs known
 * @todos   -
 * @section DESCRIPTION
 *          This class is implementing a thread pool by using an underlying threadsafe queue.
 *          The pool is using a wrapper around the std::future class to add asynchronous behaviour.
 *          There are std::thread::hardware_concurrency() - 1 threads created.
 */

#pragma once

#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/types.hh"
#include "queue.hh"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace Pool
{
    class ThreadPool
    {
        private:
            class IThreadTask
            {
                public:
                    IThreadTask() = default;
                    virtual ~IThreadTask() = default;
                    IThreadTask(const IThreadTask& rhs) = delete;
                    IThreadTask& operator=(const IThreadTask& rhs) = delete;
                    IThreadTask(IThreadTask&& other) = default;
                    IThreadTask& operator=(IThreadTask&& other) = default;
        
                    virtual void execute() = 0;
            };

            template <typename Func>
            class ThreadTask : public IThreadTask
            {
                public:
                    ThreadTask(Func&& func) :
                        _func(std::move(func))
                    {}

                    ~ThreadTask() override = default;
                    ThreadTask(const ThreadTask& rhs) = delete;
                    ThreadTask& operator=(const ThreadTask& rhs) = delete;
                    ThreadTask(ThreadTask&& other) = default;
                    ThreadTask& operator=(ThreadTask&& other) = default;
        
                    void execute() override
                    {
                        _func();
                    }

                private:
                    Func _func;
            };

        public:
            /**
             * A wrapper around a std::future that adds the behavior of futures returned from std::async.
             * Specifically, this object will block and wait for execution to finish before going out of scope.
             */
            template <typename T>
            class TaskFuture
            {
                public:
                    TaskFuture(std::future<T>&& future) : 
                        _future(std::move(future))
                    {}

                    TaskFuture(const TaskFuture& rhs) = delete;
                    TaskFuture& operator=(const TaskFuture& rhs) = delete;
                    TaskFuture(TaskFuture&& other) = default;
                    TaskFuture& operator=(TaskFuture&& other) = default;
                    ~TaskFuture()
                    {
                        if (_future.valid()) 
                        {
                            _future.get();
                        }
                    }
        
                    auto get() 
                    {
                        return _future.get();
                    }

                private:
                    std::future<T> _future;
            };

        public:
            ThreadPool() : ThreadPool(std::max(std::thread::hardware_concurrency(), 2u) - 1u) {}
   
            /**
             * @brief Creates all threads for pool
             * 
             * @param numThreads the number of threads
             */
            explicit ThreadPool(const std::uint32_t numThreads) : 
                _done(false),
                _workQueue(),
                _threads()
            {
                try 
                {
                    for (std::uint32_t i = 0u; i < numThreads; ++i) 
                    {
                        _threads.emplace_back(&ThreadPool::worker, this);
                    }
                    TRACE("All threads started successfully.");
                }
                catch (...)
                {
                    destroy_all();
                    TRACE("Creation of threadpool did not succeed - all threads destroyed.");
                    throw;
                }
            }
    
            ThreadPool(const ThreadPool& rhs) = delete;
            ThreadPool& operator=(const ThreadPool& rhs) = delete;
            ~ThreadPool()
            {
                destroy_all();
            }
    
            /**
             * @brief Submit a job to be run by the thread pool.
             *  
             */
            template <typename Func, typename... Args>
            auto submit(Func&& func, Args&&... args)
            {
                auto boundTask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
                using ResultType = std::result_of_t<decltype(boundTask)()>;
                using PackagedTask = std::packaged_task<ResultType()>;
                using TaskType = ThreadTask<PackagedTask>;
    
                PackagedTask task(std::move(boundTask));
                TaskFuture<ResultType> result(task.get_future());
                _workQueue.push(std::make_unique<TaskType>(std::move(task)));
                TRACE("New job submitted to Threadpool.");
                return result;
            }

        private:
            /**
             * @brief Constantly running function each thread uses to acquire work items from the queue.
             * 
             */
            void worker()
            {
                while (!_done)
                {
                    std::unique_ptr<IThreadTask> pTask(nullptr);
                    if (_workQueue.waitPop(pTask))
                    {
                        pTask->execute();
                    }
                }
            }

            /**
             * @brief Invalidates the queue and joins all running threads.
             * 
             */
            void destroy_all()
            {
                _done = true;
                _workQueue.invalidate();
                for (auto& thread : _threads)
                {
                    if (thread.joinable())
                    {
                        thread.join();
                    }
                }
            }

        private:
            std::atomic_bool _done;
            ThreadQueue<std::unique_ptr<IThreadTask>> _workQueue;
            std::vector<std::thread> _threads;
    };

    namespace Default
    {
        /**
         * @brief Get the ThreadPool singleton instance
         * 
         * @return ThreadPool& the instance
         */
        inline ThreadPool& getInstance()
        {
            static ThreadPool defaultPool;
            return defaultPool;
        }

        /**
         * @brief Submit a job to the ThreadPool
         * 
         */
        template <typename Func, typename... Args>
        inline auto submitJob(Func&& func, Args&&... args)
        {
            return getInstance().submit(std::forward<Func>(func), std::forward<Args>(args)...);
        }
    } // namespace Default
} // namespace Pool
