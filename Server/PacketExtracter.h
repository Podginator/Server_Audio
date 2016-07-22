#pragma once

#include "stdadfx.h"
#include "InputHandler.h"

// Extract the Packet. 
// Error handling here. 
extern std::shared_ptr<Packet> extractPacket(char* msg, const size_t& packetSize) {
  std::shared_ptr<Packet> packet = std::make_shared<Packet>();
  memcpy(packet.get(), msg, sizeof(Packet));

  if (packet.get() == nullptr) {
    packet = nullptr;
  }

  return packet; 
}
