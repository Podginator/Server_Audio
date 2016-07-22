#pragma once
#include "ClosingServerHandler.h"

//Constructor: 
// Takes a Pointer to the Server Manager calling Object. 
// The Socket Associated to this handler.
CloserServerHandler::CloserServerHandler(ServerManager* servManager, std::shared_ptr<ClientManager> associatedClient) : InputHandler(Type::CLOSE) {
  mServerManager = servManager;
  mClient = associatedClient;
}

//
// <Method> 
//  getResponse
// <Summary> 
//  return a Response Struct to send back.
// @param sentMessage the Message we have been set.
void CloserServerHandler::handlePacket(const std::shared_ptr<Packet>& sentMessage) {
  if (sentMessage->type == Type::CLOSE) {
    mServerManager->removeSocket(mClient);
  }
}
