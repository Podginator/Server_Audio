#pragma once
#include "stdadfx.h"
#include "Event.h"
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <map>
#include <typeinfo>
#include <typeindex>

// The function that constitutes a listener. This will get the
// Call back.
std::function<void(const Event* const evt)> typedef Listener; 

/**
* The Event Handler. 
*/
class EventHandler {
public:

  //
  //  <Member>
  //    Get Instance
  //  <Summary> 
  //    Gets the singleton instance
  //    Of this class.
  // @return a pointer to itself. 
  static EventHandler* getInstance();

  //  <Member>
  //    Add Event.
  //  <Summary> 
  //    Adds an event to the event queue, then notifies the event
  //    thread to pass it on. 
  // @param evt The event to send out. 
  void addEvent(Event* evt);

  //  <Member>
  //    Add Listener
  //  <Summary> 
  //    Adds a listener. 
  // @param T
  // @param evt The event to send out. 
  template<typename T,
    typename = std::enable_if < std::is_base_of<Event, T>::value >>
  void addListener(std::function<void(const T* event)> listen) {
  std::unique_lock<std::mutex> vectLock(listenerAdd, std::defer_lock);
  vectLock.lock();
  //Cast the Void pointer to the correct type in the listener.
  auto listener = [=](const Event* const evt) {
    listen(static_cast<const T* const>(evt));
  };
  mListeners[typeid(T)].push_back(listener);
  vectLock.unlock();
}


  //
  //  <Member> 
  //    removeListener
  //  <Summary>
  //    Remove the listener.
  //
  //@param listen the listener to remove.
  void removeListener(const Listener& listen);

private:

  //
  //  <Member> 
  //    ctor
  //  <Summary>
  //    Start the event loop.
  EventHandler();

  //
  //  <Member> 
  //    Loops through the event, dequeuing events
  //    and sending them to the listeners.
  //  <Summary>
  //    Start the event loop.
  void beginEventLoop();
 
  //The listeners
  std::map<std::type_index, std::vector<Listener>> mListeners;
  //Lock variable. 
  std::mutex mMutex;
  // The lock around the listeners.
  std::mutex listenerAdd;
  //The Event Adding lock
  std::mutex evtAdd; 
  //The queue of events.
  std::queue<Event*> mEventQueue;
  //The Condition variable has events.
  std::condition_variable mHasItems; 
};
