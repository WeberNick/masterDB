/*

#include "queue.hh"

template <class T>
ThreadQueue<T>::ThreadQueue() : _valid(true), _mutex(), _queue(), _condition() {
}

template <class T>
ThreadQueue<T>::~ThreadQueue() {
  invalidate();
}

template <class T>
void ThreadQueue<T>::invalidate() {
  std::lock_guard<std::mutex> lock(_mutex);
  _valid = false;
  _condition.notify_all();
}

template <class T>
bool ThreadQueue<T>::isValid() const {
  std::lock_guard<std::mutex> lock(_mutex);
  return _valid;
}

template <class T>
void ThreadQueue<T>::clear() {
  std::lock_guard<std::mutex> lock(_mutex);
  while (!_queue.empty()) {
    _queue.pop();
  }
  _condition.notify_all();
}

template <class T>
bool ThreadQueue<T>::isEmpty() const {
  std::lock_guard<std::mutex> lock(_mutex);
  return _queue.empty();
}

template <class T>
void ThreadQueue<T>::push(T value) {
  std::lock_guard<std::mutex> lock(_mutex);
  _queue.push(std::move(value));
  _condition.notify_one();
}

template <class T>
bool ThreadQueue<T>::waitPop(T &out) {
  std::unique_lock<std::mutex> lock(_mutex);
  _condition.wait(lock, [this]() { return !_queue.empty() || !_valid; });

  if (!_valid) { return false; }
  out = std::move(_queue.front());
  _queue.pop();
  return true;
}

template <class T>
bool ThreadQueue<T>::tryPop(T &out) {
  std::lock_guard<std::mutex> lock(_mutex);
  if (_queue.empty() || !_valid) { return false; }
  out = std::move(_queue.front());
  _queue.pop();
  return true;
}

*/