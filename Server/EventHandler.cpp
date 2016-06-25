#include "EventHandler.h"

//
//  <Member>
//    Get Instance
//  <Summary> 
//    Gets the singleton instance
//    Of this class.
// @return a pointer to itself. 
EventHandler* EventHandler::getInstance() {
  static EventHandler* me = nullptr;

  if (me == nullptr) {
    me = new EventHandler();
  }

  return me;
}

//  <Member>
//    Add Event.
//  <Summary> 
//    Adds an event to the event queue, then notifies the event
//    thread to pass it on. 
// @param evt The event to send out. 
void EventHandler::addEvent(Event* evt) {
  //Lock guard the Mutex, Avoid calling the Listeners before we need to. 
  std::lock_guard<std::mutex> lockGuard(mMutex);
  //also lock for adding events, as these collections aren't thread safe. 
  std::unique_lock<std::mutex> vectLock(evtAdd, std::defer_lock);
  vectLock.lock();
  mEventQueue.push(evt);
  mHasItems.notify_all();
  vectLock.unlock();
}

//
//  <Member> 
//    removeListener
//  <Summary>
//    Remove the listener.
//
//@param listen the listener to remove.
void removeListener(const Listener& listen) {
  //Go through and remove the listener.
}


//
//  <Member> 
//    ctor
//  <Summary>
//    Start the event loop.
EventHandler::EventHandler() {
  //Begin the event loop.
  std::thread thread(&EventHandler::beginEventLoop, this);
  thread.detach();
}

//
//  <Member> 
//    Loops through the event, dequeuing events
//    and sending them to the listeners.
//  <Summary>
//    Start the event loop.
void EventHandler::beginEventLoop() {
  while (true) {
    std::unique_lock<std::mutex> evtLock(mMutex, std::defer_lock);
    evtLock.lock();
    //Wait until we have any 
    mHasItems.wait(evtLock, [&] {return mEventQueue.size() > 0; });
    Event* evt = mEventQueue.front();
    mEventQueue.pop();

    std::lock_guard<std::mutex> lockGuard(listenerAdd);
    //Alert each listener
    for each (Listener listener in mListeners[typeid(*evt)]) {
      listener(evt);
    }
    delete evt;
    evtLock.unlock();
  }
}