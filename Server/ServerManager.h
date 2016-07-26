#pragma once

#include "stdadfx.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "InputHandler.h"
#include "SocketManagerFactory.h"
#include <set> 

//
// The Manager class to manage the Listening Server and Connection
//
class ServerManager {

public:

	//
	// <Method>
	//		Constructor
	// <Summary>
	//		Constructs the Server Manager.
	// @param serPtr the Server Socket we want to listen on.
	//
  ServerManager(shared_ptr<ServerSocket>& serPtr, 
    shared_ptr<SocketManagerFactory>);

	//Destructor.
  ~ServerManager();

	//
	// <Method>
	//		listen
	// <Summary>
	//   Accept a connection that is attempting to connection
	//   Return a unique pointer to the Socket that is created.
	// @return Pointer to a Socket.
	//
  void listen();


private:
	//The Server Socket we're listening on.
  shared_ptr<ServerSocket> mServerSocket;

  shared_ptr<SocketManagerFactory> mManagerFactory;

  //
  // <Method>
  //		acceptConnection
  // <Summary>
  //   Accept a connection that is attempting to connection
  //   Return a unique pointer to the Socket that is created.
  // @return Pointer to a Socket.
  //
  void acceptConnection(shared_ptr<Socket> socket);

  //
  // <Method>
  //		getConnections
  // <Summary>
  //   Listen to the Server Socket, accepting any 
  // @return Pointer to a Socket.
  //
  void getConnections();

};