#pragma once
#include "InputHandler.h"
#include "ServerManager.h"
#include "Socket.h"

class ServerManager;
/**
* The AudioServer Handler
*/
class CloserServerHandler : public InputHandler
{
public:

  //Constructor: 
  // Takes a Pointer to the Server Manager calling Object. 
  // The Socket Associated to this handler.
  CloserServerHandler(ServerManager* servManager, std::shared_ptr<Socket> associatedSocket);

  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
  void handlePacket(const Packet& sentMessage);
private:
  //Pointer to the Server Manager caller object. 
  ServerManager* mServerManager;

  //The Shared ptr to the socket.
  std::shared_ptr<Socket> mSocket;
};