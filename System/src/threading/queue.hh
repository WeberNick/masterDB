/**
 * @file    queue.hh
 * @author  Nicolas Wipfler (nwipfler@mail.uni-mannheim.de)
 *          Aljoscha Narr (alnarr@mail.uni-mannheim.de)
 * @brief   Class implementing a threadsafe queue for submitting jobs to the thread pool.
 * @bugs    TBD
 * @todos   TBD
 */

#ifndef QUEUE_HH
#define QUEUE_HH

#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>

template<typename T>
class ThreadQueue
{
  private:
    ThreadQueue();
    ThreadQueue(const ThreadQueue &aQueue) = delete;
    ThreadQueue &operator=(const ThreadQueue &aQueue) = delete;
    ~ThreadQueue();

  public:
    void push(T value);
    bool tryPop(T& out);
    bool waitPop(T& out);
    void clear();
    bool isEmpty() const;
    bool isValid() const;
    void invalidate();
  
  private:
    std::atomic_bool _valid;
    mutable std::mutex _mutex;
    std::queue<T> _queue;
    std::condition_variable _condition;
};

#endif