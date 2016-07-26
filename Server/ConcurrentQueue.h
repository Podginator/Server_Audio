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

    unique_lock<mutex> lk(mMutex);

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
    unique_lock<mutex> lk(mMutex);
    res = mQueue.size();
    lk.unlock();
    return res;
  }

  void push(const T& item) {
    unique_lock<mutex> lk(mMutex);
    mQueue.push(item);
    lk.unlock();
    mCondVar.notify_all();
  }


  void clear() {
    unique_lock<mutex> lk(mMutex);
    mQueue.clear();
    lk.unlock();
  }

private: 
  queue<T> mQueue; 

  mutex mMutex; 

  condition_variable mCondVar;

};