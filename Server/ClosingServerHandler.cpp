#pragma once
#include "ClosingServerHandler.h"

//Constructor: 
// Takes a Pointer to the Server Manager calling Object. 
// The Socket Associated to this handler.
CloserServerHandler::CloserServerHandler(ServerManager* servManager, std::shared_ptr<Socket> associatedSocket) {
  mServerManager = servManager;
  mSocket = associatedSocket;
}

//
// <Method> 
//  getResponse
// <Summary> 
//  return a Response Struct to send back.
// @param sentMessage the Message we have been set.
void CloserServerHandler::handlePacket(const Packet & sentMessage) {
  // if close then close.
}
