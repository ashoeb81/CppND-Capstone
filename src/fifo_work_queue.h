#ifndef FIFO_WORK_QUEUE_H
#define FIFO_WORK_QUEUE_H

#include <mutex>
#include <deque>

// Templated First-In-First-Out (FIFO) queue supporting the addition
// (push) and removal (pop) of items in multi-threaded setting
// using locks.
template <class T>
class FIFOWorkQueue
{
public:
  // Removes and returns an item T from the queue.
  T Pop();

  // Adds an item T to the queueu.
  void Push(T &&msg);

private:
  // Mutex used to lock the queue during push and pop operations.
  std::mutex _mutex;

  // Double-ended queue used to implement FIFO item retrieval.
  std::deque<T> _queue;
};

template <class T>
T FIFOWorkQueue<T>::Pop()
{
  // Queue modification under the lock
  std::lock_guard<std::mutex> uLock(_mutex);

  // If the queueu is not empty,
  // then return the element at the front
  // using std::move semantics.
  T msg;
  if (!_queue.empty())
  {
    msg = std::move(_queue.front());
    _queue.pop_front();
  }
  return msg;
}

template <class T>
void FIFOWorkQueue<T>::Push(T &&msg)
{
  // Queue modification under the lock
  std::lock_guard<std::mutex> uLock(_mutex);

  // Push most recent elements to the back fo the queue.
  // Use std::move to avoid copies and give queue
  // full ownership of object.
  _queue.push_back(std::move(msg));
}

#endif