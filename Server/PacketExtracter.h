#pragma once

#include "stdadfx.h"
#include "InputHandler.h"

// Extract the Packet. 
extern Packet extractPacket(char* &msg, const size_t& packetSize) {

  //Construct items.
  Packet packet; 
  packet.type = NO_OPP;
  char* buffer = nullptr;

  //Is the packet greater than 0.
  if (packetSize > 0) {
    char typeChar = msg[0];
    Type type = static_cast<Type>((int)typeChar - '0');
    switch (type) {
    case Type::NO_OPP: 
    case Type::AUDIO:
    case Type::EXIT:
    case Type::FRIEND:
    case Type::TRACK:
    case Type::FILELIST:
    case Type::ACKNOWLEDGE:
      // Remove the type information.
      packet.size = packetSize - 1;
      packet.type = type;
      //To do: Goodness no this is shite.
      buffer = new char[packetSize];
      memcpy(buffer, msg + 1, packetSize);
      buffer[packetSize - 1] = '\0';
      packet.packetData = buffer;
      break;
    default:
      break;
    }
  }

  //Now delete the char.
  delete msg;

  return packet;
}


extern bool retrieveParametersFromPacket(const Packet& packet, size_t nArgs, ...) {
  return nullptr;
}