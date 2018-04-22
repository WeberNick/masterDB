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

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class ThreadQueue {
  public:
    ~ThreadQueue(void) {
        invalidate();
    }

    bool tryPop(T& out) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.empty() || !_valid) {
            return false;
        }
        out = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    bool waitPop(T& out) {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, [this]() {
            return !_queue.empty() || !_valid;
        });

        if (!_valid) {
            return false;
        }
        out = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    void push(T value) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(value));
        _condition.notify_one();
    }

    bool empty(void) const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    void clear(void) {
        std::lock_guard<std::mutex> lock(_mutex);
        while (!_queue.empty()) {
            _queue.pop();
        }
        _condition.notify_all();
    }

    void invalidate(void) {
        std::lock_guard<std::mutex> lock(_mutex);
        _valid = false;
        _condition.notify_all();
    }

    bool isValid(void) const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _valid;
    }

  private:
    std::atomic_bool _valid{ true };
    mutable std::mutex _mutex;
    std::queue<T> _queue;
    std::condition_variable _condition;
};