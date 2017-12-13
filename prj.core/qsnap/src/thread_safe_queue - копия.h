#pragma once
#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

#include <memory>
#include <condition_variable>

namespace XThreads // another name for namespace?
{
// from Anthony Williams book "C++ Concurrency in Action"
class join_threads
{
  std::vector<std::thread>& threads;

public:
  explicit join_threads(std::vector<std::thread>& threads_):
    threads(threads_)
  {}
  ~join_threads()
  {
    for(unsigned long i = 0; i < threads.size(); ++i)
    {
      if ( threads[i].joinable() )
        threads[i].join();
    }
  }
};

template<typename T>
class threadsafe_queue
{
private:
  mutable std::mutex mut;
  std::queue<T> data_queue;
  std::condition_variable data_cond;
public:
  threadsafe_queue()
  {}
  threadsafe_queue(threadsafe_queue const& other)
  {
    std::lock_guard<std::mutex> lk(other.mut);
    data_queue=other.data_queue;
  }
  void push(T new_value)
  {
    std::lock_guard<std::mutex> lk(mut);
    data_queue.push(new_value);
    data_cond.notify_one();
  }
  void wait_and_pop(T& value)
  {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk,[this]{return !data_queue.empty();});
    value=data_queue.front();
    data_queue.pop();
  }
  std::shared_ptr<T> wait_and_pop()
  {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk,[this]{return !data_queue.empty();});
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }

  bool try_pop(T& value)
  {
    std::lock_guard<std::mutex> lk(mut);
    if(data_queue.empty())
      return false;
    value=data_queue.front();
    data_queue.pop();
    return true;
  }
  std::shared_ptr<T> try_pop()
  {
    std::lock_guard<std::mutex> lk(mut);
    if(data_queue.empty())
      return std::shared_ptr<T>();
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }
  bool empty() const
  {
    std::lock_guard<std::mutex> lk(mut);
    return data_queue.empty();
  }
};

class XThread_pool
{
  std::atomic_bool done;
  std::atomic_bool stopped;

  threadsafe_queue<std::function<void()> > work_queue;

  std::vector<std::thread> threads;
  join_threads joiner;

  std::mutex m_queMutex;

  void worker_thread()
  {
    while(!done)
    {
      std::function<void()> task;
      if (work_queue.try_pop(task))
      {
        task();
      }
      else
      {
        empty = true;
        std::this_thread::yield();
      }
    }
    stopped = true;
  }
public:
  std::atomic_bool empty;

  XThread_pool(int nThreads) :
    joiner(threads)
  {
    std::atomic_init(&done, false);
    std::atomic_init(&empty, true);
    std::atomic_init(&stopped, false);
    unsigned const thread_count = (nThreads <= 0) 
       ? std::thread::hardware_concurrency()
       : nThreads;
    try
    {
      for(unsigned i = 0; i < thread_count; ++i)
      {
        threads.push_back(
          std::thread(&XThread_pool::worker_thread, this));
      }
    }
    catch(...)
    {
      done = true;
      throw;
    }
  }
  ~XThread_pool()
  {
    done = true;
    while(!stopped)
    {
      std::this_thread::yield();
    }
  }
  template<typename FunctionType>
  void submit(FunctionType f)
  {
    work_queue.push(std::function<void()>(f));
    empty = false;
  }

  bool isEmpty()
  {
    return work_queue.empty();
  }
private:
  XThread_pool(const XThread_pool&);
};

};

#endif // THREAD_SAFE_QUEUE_H