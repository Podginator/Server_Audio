#pragma once

#include "stdadfx.h"

// Type Enum, Bit Flags as we want the handlers
// To be able to handle a lot of them.
enum Type {
  NO_OPP = 0,
  ACKNOWLEDGE = 1 << 1,
  EXIT = 1 << 2,
  AUDIO = 1 << 3,
  FILELIST = 1 << 4,
  TRACK = 1 << 5,
  CLOSE = 1 << 6,
  FRIEND = 1 << 7
};

//Expose bitwise operators.
inline Type operator~ (Type a) { return (Type)~(int)a; }
inline Type operator| (Type a, Type b) { return (Type)((int)a | (int)b); }
inline Type operator& (Type a, Type b) { return (Type)((int)a & (int)b); }
inline Type operator^ (Type a, Type b) { return (Type)((int)a ^ (int)b); }
inline Type& operator|= (Type& a, Type b) { return (Type&)((int&)a |= (int)b); }
inline Type& operator&= (Type& a, Type b) { return (Type&)((int&)a &= (int)b); }
inline Type& operator^= (Type& a, Type b) { return (Type&)((int&)a ^= (int)b); }

/**
* The response object.
*/
struct Packet {
public:

  // The maximum available packet size to send .
  // Note: Not the sizeof(Packet), but instead the size
  // Of the data stored in the packet.
  const int static maxPacketSize = 1024;

  //Type of the Packet. 
  Type type;

  // Size of the Data. 
  int size;

  //Byte Array, max length set to 1024.
  byte packetData[maxPacketSize];

  Packet() : type(Type::NO_OPP), size(0), packetData() {};

  Packet(Type type, int size, byte* data) {
    this->type = type; 
    this->size = size; 

    if (data != nullptr) {
      //Todo memcpy?
      memcpy(packetData, data, maxPacketSize);
    }
  }

};

/**
* The Server Handler Interface.
*/
class InputHandler
{
public:

  InputHandler(Type listenFor) {
    mListenFor = listenFor;
  }

  Type listensFor(Type type) {
    return type & mListenFor;
  }

  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
	virtual void handlePacket(const std::shared_ptr<Packet>& packet) = 0;
private:
  Type mListenFor;
};