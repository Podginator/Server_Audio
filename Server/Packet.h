#pragma once
#include "stdadfx.h"

// Type Enum, Bit Flags as we want the handlers
// To be able to handle a lot of them.
enum Type {
  INVALID = 0,
  TRACK = 1 << 1,
  AUDIO = 1 << 2,
  HEADER = 1 << 3,
  FILELIST = 1 << 4,
  FRIEND_REQ = 1 << 5,
  USER_UPDATE = 1 << 6,
  EXIT = 1 << 7,
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

  // Size of the Data. 
  int size;

  //Type of the Packet. 
  Type type;

  //Byte Array, max length set to 1024.
  byte packetData[maxPacketSize];

  Packet() : type(Type::INVALID), size(0), packetData() {};

  Packet(Type type, int size, byte* data) : type(type), size(size) {
    if (data != nullptr) {
      std::copy(data, data + size, packetData);
    }
  }

};