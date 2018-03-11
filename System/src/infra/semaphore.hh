#pragma once

#include <mutex>
#include <condition_variable>
#include <cstddef>

class Semaphore
{
    public:
        explicit Semaphore(std::size_t aCount = 0) : _mtx(), _cv(), _count(aCount){}
        Semaphore(const Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;
        ~Semaphore();

    public:
        inline void notify()
        {
            std::unique_lock<std::mutex> lock(_mtx);
            _count++;
            _cv.notify_one();
        }

        inline void wait()
        {
            std::unique_lock<std::mutex> lock(_mtx);

            while(_count == 0)
            {
                _cv.wait(lock);
            }
            _count--;
        }

    private:
        std::mutex _mtx;
        std::condition_variable _cv;
        std::size_t _count;
};
