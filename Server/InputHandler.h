#pragma once

#include "stdadfx.h"

// Type Enum, Bit Flags as we want the handlers
// To be able to handle a lot of them.
enum Type {
  INVALID = 0,
  ACKNOWLEDGE = 1 << 1,
  TRACK = 1 << 2,
  AUDIO = 1 << 3,
  HEADER = 1 << 4,
  FILELIST = 1 << 5,
  FRIEND_REQ = 1 << 6,
  USER_UPDATE = 1 << 7,
  EXIT = 1 << 8,
  First = INVALID,
  Last = EXIT
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

  Packet() : type(Type::INVALID), size(0), packetData() {};

  ~Packet() {}

  Packet(Type type, int size, byte* data) {
    this->type = type; 
    this->size = size; 

    if (data != nullptr) {      
      std::copy(data, data + size, packetData);
    }
  }

};

/**
* The Server Handler Interface.
*/
class InputHandler
{
public:

  //Constructor.
  //Can call with either 1 Type Ie: InputHandler(Type::Audio)
  //Or can call with multiple, IE InputHandler(Type::Audio & Type::FileList)
  InputHandler(Type listenFor) {
    mListenFor = listenFor;
  }

  // Returns True if Type & Listen for > 1
  bool listensFor(Type type) {
    return (type & mListenFor) != 0;
  }

  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
  virtual void handlePacket(const Packet& packet) = 0;
private:
  // What type this is listening for.
  Type mListenFor;
};