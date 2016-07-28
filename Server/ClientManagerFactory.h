#pragma once
#include "stdadfx.h"
#include "Socket.h"
#include "ClientManager.h"

// Interface to create a pointer to a "Client Manager" this is what will be 
// Created when accepting an incoming connection from a Socket. 
class ClientManagerFactory {
public:

  // Factory method to create a Socket Manager. 
  // Anything created from here gains unique ownership.
  virtual unique_ptr<ClientManager> createClientManager(unique_ptr<Socket> socket) = 0;
};
