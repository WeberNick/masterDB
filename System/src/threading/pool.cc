#include "pool.hh"

ThreadPool::ThreadPool() :
    ThreadPool(std::max(std::thread::hardware_concurrency(), 2u) - 1u)
{}

/*

not accessible ??? 

ThreadPool::ThreadPool(const std::uint32_t numThreads) :
    _done(false),
    _queue(),
    _threads()
{
    try
            {
                for(std::uint32_t i = 0u; i < numThreads; ++i)
                {
                    _threads.emplace_back(&ThreadPool::worker_thread, this);
                }
            }
            catch(...)
            {
                destroy_all();
                throw;
            }
}

ThreadPool::~ThreadPool()
{
    destroy_all();
}

*/

template <typename Func, typename... Args>
        auto ThreadPool::submit(Func&& func, Args&&... args)
        {
            /*
                TODO
            */
        }

void ThreadPool::worker_thread()
{
    while(!_done)
            {
                std::unique_ptr<IThreadTask> pTask(nullptr);
                if(_queue.waitPop(pTask))
                {
                    pTask->execute();
                }
            }
}

void ThreadPool::destroy_all()
{
    _done = true;
            _queue.invalidate();
            for(auto& thread : _threads)
            {
                if(thread.joinable())
                {
                    thread.join();
                }
            }
}