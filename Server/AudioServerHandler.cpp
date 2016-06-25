#include "AudioServerHandler.h"

AudioServerHandler::AudioServerHandler(std::shared_ptr<ConcurrentQueue<Packet>> conQue) {
    mConQueue = conQue;
}

//
// <Method> 
//  getResponse
// <Summary> 
//  return a Response Struct to send back.
// @param sentMessage the Message we have been set.
void AudioServerHandler::handlePacket(const Packet& sentMessage) {
  //Packet responseCode = { 0 };
  //responseCode.response = "Hello";


  //mConQueue->push(responseCode);
  //return responseCode;
}