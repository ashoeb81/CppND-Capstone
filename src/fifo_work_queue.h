#ifndef FIFO_WORK_QUEUE_H
#define FIFO_WORK_QUEUE_H

#include <mutex>
#include <deque>
#include <condition_variable>

template <class T>
class FIFOWorkQueue
{
public:
    T pop();
    void push(T&& msg);

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _queue;
};

template <class T>
T FIFOWorkQueue<T>::pop()
{
    // perform queue modification under the lock
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.front());
    _queue.pop_front();

    return msg;
}

template <class T>
void FIFOWorkQueue<T>::push(T&& msg)
{    
    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);
    _queue.push_back(msg);
    _cond.notify_one();
}

#endif