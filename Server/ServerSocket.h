#pragma once

#include "stdadfx.h"
#include "Socket.h"

/**
* The Server Instance that Listens to
* Incoming Connections.
*
* We only need one of these, therefore it's defined
* As a singleton.
*/
class ServerSocket {
private:
  //The port to connect on.
  int mPort;
  //The address we wish to connect to
	std::string address;
  //The Socket File Description
	int socketFileDesc;


  // <Method>
  //		setupServerAddressStruct
  // <Summary>
  //		Sets up pthe Sockaddr_in 
  // @param ref The Socket_addr_in to modify.
  //
  void setupServerHints(addrinfo& ref);

public:

  // <Method>
  //		Constructor
  // <Summary>
  //		Constructs the Server Objects
  // @param hostName the host name to connect to (Ie: IP-Port)
  // @param port the port to listen to
  //
  ServerSocket(const std::string& hostName, int port);
	
  //Destructor.
  ~ServerSocket();

	// <Method>
	//  accept
	// <Summary>
	//   Accept a connection that is attempting to connection
	//   Return a unique pointer to the Socket that is created.
	// @return Pointer to a Socket.
	//
  std::shared_ptr<Socket> acceptSocket();

	//
	// <Method>
	// 	Begin Listening
	// <Summary>
	//   Starts listening on the Socket.
	//
  bool begin();

  // 
  // <Method>
  //  Close
  // <Summary> 
  //  Close the socket.
  //
  void close();
};