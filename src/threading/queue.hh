/**
 * @file    queue.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *          Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 * @brief   Class implementing a threadsafe queue for submitting jobs to the
 * thread pool.
 * @bugs    TBD
 * @todos   TBD
 */

#pragma once

#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/types.hh"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * Thread-safe queue for adding new tasks.
 * Implemented as wrapper around basic queue
 */
template <typename T>
class ThreadQueue
{
    public:
        /**
         * Destructor.
         */
        ~ThreadQueue(void)
        {
            invalidate();
        }
    
        /**
         * Trying to get the first value in the queue.
         * Returns true if a value was successfully written to the out parameter, false otherwise.
         */
        bool tryPop(T& out)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_queue.empty() || !_valid)
            {
                return false;
            }
            out = std::move(_queue.front());
            _queue.pop();
            return true;
        }
    
        /**
         * Get the first value in the queue.
         * Will block until a value is available unless clear is called or the instance is destructed.
         * Returns true if a value was successfully written to the out parameter, false otherwise.
         */
        bool waitPop(T& out)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _condition.wait(lock, [this]()
            {
                return !_queue.empty() || !_valid;
            });
            if (!_valid)
            {
                return false;
            }
            out = std::move(_queue.front());
            _queue.pop();
            return true;
        }
    
        /**
         * Push a new value onto the queue.
         */
        void push(T value)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _queue.push(std::move(value));
            _condition.notify_one();
        }
    
        /**
         * Check whether or not the queue is empty.
         */
        bool empty(void) const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _queue.empty();
        }
    
        /**
         * Clear all items from the queue.
         */
        void clear(void)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            while (!_queue.empty())
            {
                _queue.pop();
            }
            _condition.notify_all();
        }
    
        /**
         * Invalidate the queue.
         */
        void invalidate(void)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _valid = false;
            _condition.notify_all();
        }
    
        /**
         * Returns whether or not this queue is valid.
         */
        bool isValid(void) const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _valid;
        }

    private:
        std::atomic_bool _valid{ true };
        mutable std::mutex _mutex;
        std::queue<T> _queue;
        std::condition_variable _condition;
};
