/**
 * @file    pool.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *          Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 * @brief   Class implementing a thread pool.
 * @bugs    TBD
 * @todos   TBD
 */

#ifndef POOL_HH
#define POOL_HH

#include "queue.hh"

#include <atomic>
#include <future>
#include <thread>
#include <vector>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>
#include <memory>
#include <algorithm>

class ThreadPool {
  
  public:
    ThreadPool();
    explicit ThreadPool(const std::uint32_t numThreads);
    ThreadPool(const ThreadPool& rhs) = delete;
    ThreadPool& operator=(const ThreadPool& rhs) = delete;
    ~ThreadPool();

    template <typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args);

  protected:
    class IThreadTask
        {
        public:
            IThreadTask(void) = default;
            virtual ~IThreadTask(void) = default;
            IThreadTask(const IThreadTask& rhs) = delete;
            IThreadTask& operator=(const IThreadTask& rhs) = delete;
            IThreadTask(IThreadTask&& other) = default;
            IThreadTask& operator=(IThreadTask&& other) = default;

            virtual void execute() = 0;
        };

        template <typename Func>
        class ThreadTask: public IThreadTask
        {
        public:
            ThreadTask(Func&& func)
                :_func{std::move(func)}
            {
            }

            ~ThreadTask(void) override = default;
            ThreadTask(const ThreadTask& rhs) = delete;
            ThreadTask& operator=(const ThreadTask& rhs) = delete;
            ThreadTask(ThreadTask&& other) = default;
            ThreadTask& operator=(ThreadTask&& other) = default;

            inline void execute() override
            {
                _func();
            }

        private:
            Func _func;
        };

  private:
    void worker_thread();
    void destroy_all();

  private:
    std::atomic_bool _done;
    ThreadQueue<std::unique_ptr<IThreadTask>> _queue;
    std::vector<std::thread> _threads;
};

#endif