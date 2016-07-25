#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

//Wrapper around a standard Queue. 
template <typename T> 
class ConcurrentQueue {
public: 


  T pop() {

    std::unique_lock<std::mutex> lk(mMutex);

    while (mQueue.empty()) {
      mCondVar.wait(lk);
    }
    //Get the item from the front of the queue.
    T res = mQueue.front();
    //Then pop it. 
    mQueue.pop();

    return res; 
  }

  size_t size() {
    size_t res = 0;
    std::unique_lock<std::mutex> lk(mMutex);
    res = mQueue.size();
    lk.unlock();
    return res;
  }

  void push(const T& item) {
    std::unique_lock<std::mutex> lk(mMutex);
    mQueue.push(item);
    lk.unlock();
    mCondVar.notify_all();
  }

private: 
  std::queue<T> mQueue; 

  std::mutex mMutex; 

  std::condition_variable mCondVar;

};