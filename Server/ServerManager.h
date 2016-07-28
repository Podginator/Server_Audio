#pragma once

#include "stdadfx.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "InputHandler.h"
#include "ClientManagerFactory.h"
#include <set> 

// The Manager class to manage the Listening Server and Connection
class ServerManager {

public:

  // Constructor 
  //  serPtr : The Server socket we wish to listen on
  //  factory : The factory to create a Client Manager on.
  ServerManager(unique_ptr<ServerSocket>& serPtr, 
    unique_ptr<ClientManagerFactory>& factory);

  //Destructor.
  ~ServerManager();

  // Listen to the server. 
  // Accept connections.ClientManagerFactory
  void listen();

private:

  //The Server Socket we're listening on.
  unique_ptr<ServerSocket> mServerSocket;

  // The Pointer to the facotory to client from. 
  unique_ptr<ClientManagerFactory> mManagerFactory;

  // Accept the Connection.
  void acceptConnection(unique_ptr<Socket> socket);

  // Get any incoming connections. Create a client from it. 
  void getConnections();

};