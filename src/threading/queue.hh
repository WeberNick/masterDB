/**
 * @file    queue.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *          Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 * @brief   Class implementing a threadsafe queue for submitting jobs to the thread pool. Implemented as wrapper around basic queue.
 * @bugs    Currently no bugs known
 * @todos   -
 * @section DESCRIPTION
 *          This class is implementing a templated waiting queue in a threadsafe manner by using a wrapper around the general queue implementation.
 *          The queue is used to submit jobs to the overlying thread pool.
 */

#pragma once

#include "../infra/exception.hh"
#include "../infra/trace.hh"
#include "../infra/types.hh"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class ThreadQueue
{
    public:
    /**
     * @brief   Destruction by invalidating the queue.
     * 
     */
        ~ThreadQueue()
        {
            invalidate();
        }
        /**
         * @brief       Trying to get the first value in the queue.
         * 
         * @param out   Output parameter in which the value is written.
         * @return      True if value was successfully written to the out parameter, false otherwise
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
         * @brief       Get the first value in the queue.  Will block until a value is available unless clear is called or the instance is destructed.
         * 
         * @param out   Output parameter in which the value is written.
         * @return      True if if a value was successfully written to the out parameter, false otherwise.
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
         * @brief           Push a new value onto the queue.
         * 
         * @param value     Value to be pushed on the queue
         */
        void push(T value)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _queue.push(std::move(value));
            _condition.notify_one();
        }
        /**
         * @brief   Check whether or not the queue is empty.
         * 
         * @return  True if queue is empty, false otherwise
         */
        bool empty() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _queue.empty();
        }
        /**
         * @brief   Clear all items from the queue.
         * 
         */
        void clear()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            while (!_queue.empty())
            {
                _queue.pop();
            }
            _condition.notify_all();
        }
        /**
         * @brief   Invalidate the whole queue.
         * 
         */
        void invalidate()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _valid = false;
            _condition.notify_all();
        }
        /**
         * @brief   Returns whether or not this queue is valid.
         * 
         * @return  True if queue is valid, false otherwise     
         */
        bool isValid() const
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
