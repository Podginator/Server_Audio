#pragma once

#include "stdadfx.h"
#include "InputHandler.h"

// Extract the Packet. 
// Error handling here. 
extern Packet extractPacket(char* msg, const size_t& packetSize) {
  Packet packet;
  packet.type = Type::NO_OPP;
  char* currPos = msg;
  // Okay, so check that we have the right size first.

  if (packetSize == sizeof(Packet) && msg) {
    //Then convert type
    Type type;
    currPos = msg + sizeof(Type);
    std::copy(msg, currPos, type); 

    //Ensure that we're not NO_OPP, and that we're less than or equal to the last val
    //And that we're a power of 2 (Bitflag)
    if (type != Type::NO_OPP && type <= Type::Last && (type & (type - 1))) {
      //So long as this is true we can continue. 
      int size; 
      std::copy(currPos, currPos + sizeof(int), size);
      currPos += sizeof(int);
      // AS long as the packet isn't greater than the max size we can continue.
      if (size <= Packet::maxPacketSize && size > 0)
        //Therefore we can be reasonably happy that we've got a packet, so assign.
        packet.type = type;
        packet.size = size; 
        //and apply the byte data.
        std::copy(currPos, currPos + size, packet.packetData);
      }
    }

  return packet; 
}
