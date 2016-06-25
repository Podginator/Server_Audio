#pragma once
#include "InputHandler.h"
#include "ConcurrentQueue.h"

/**
* The AudioServer Handler
*/
class AudioServerHandler : public InputHandler {
public:

  //Constructor, takes a ConcurrentQueue, which is often associated with 
  //the 
  AudioServerHandler(std::shared_ptr<ConcurrentQueue<Packet>> conQue);
  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
  void handlePacket(const Packet & sentMessage);
private: 
  //The Concurrent Queue to drain from. 
  std::shared_ptr<ConcurrentQueue<Packet>> mConQueue;
};