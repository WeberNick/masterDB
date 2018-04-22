/*

#include "pool.hh"

Pool::ThreadPool::ThreadPool() : ThreadPool{ std::max(std::thread::hardware_concurrency(), 2u) - 1u } {}

Pool::ThreadPool::ThreadPool(const std::uint32_t numThreads) : _done(false), _queue(), _threads() {
  try {
    for (std::uint32_t i = 0u; i < numThreads; ++i) {
      _threads.emplace_back(&ThreadPool::worker_thread, this);
    }
  } catch (...) {
    destroy_all();
    throw;
  }
}

template <typename Func, typename... Args>
auto Pool::ThreadPool::submit(Func&& func, Args&&... args) {
  auto boundTask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
  using T = std::result_of_t<decltype(boundTask)()>;
  using PackagedTask = std::packaged_task<T()>;
  using TaskType = ThreadTask<PackagedTask>;

  PackagedTask task{ std::move(boundTask) };
  TaskFuture<T> result{ task.get_future() };
  _queue.push(std::make_unique<TaskType>(std::move(task)));
  return result;
}

void Pool::ThreadPool::worker_thread() {
  while (!_done) {
    std::unique_ptr<IThreadTask> pTask{ nullptr };
    if (_queue.waitPop(pTask)) { pTask->execute(); }
  }
}

void Pool::ThreadPool::destroy_all() {
  _done = true;
  _queue.invalidate();
  for (auto& thread : _threads) {
    if (thread.joinable()) { thread.join(); }
  }
}
*/