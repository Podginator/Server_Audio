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
  ServerManager(std::shared_ptr<ServerSocket>& serPtr, 
    std::shared_ptr<SocketManagerFactory>);

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

  //
  // <Method>
  //		removeSocket
  // <Summary>
  //   remove the reference to the socket from the list.
  //
  void removeSocket(std::shared_ptr<Socket> socket);

private:
	//The Server Socket we're listening on.
  std::shared_ptr<ServerSocket> mServerSocket;

  std::shared_ptr<SocketManagerFactory> mManagerFactory;

	//The Set of Sockets.
	std::set<std::shared_ptr<ClientManager>> mSocketSet;

  //
  // <Method>
  //		acceptConnection
  // <Summary>
  //   Accept a connection that is attempting to connection
  //   Return a unique pointer to the Socket that is created.
  // @return Pointer to a Socket.
  //
  void acceptConnection(std::shared_ptr<Socket> socket);

  //
  // <Method>
  //		getConnections
  // <Summary>
  //   Listen to the Server Socket, accepting any 
  // @return Pointer to a Socket.
  //
  void getConnections();

};