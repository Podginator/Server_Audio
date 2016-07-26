#pragma once
#include "stdadfx.h"
#include "ClientManager.h"
#include "InputHandler.h"
#include "AudioServerHandler.h"
#include "ServerManager.h"
#include "ClosingServerHandler.h"

class ServerManager;

// Interface to create a pointer to a "Client Manager" this is what will be 
// Created when accepting an incoming connection from a Socket. 
class SocketManagerFactory {
public:
  virtual shared_ptr<ClientManager> createSocketManager(shared_ptr<Socket> socket, ServerManager* servMan) = 0;
};
