#pragma once

#include "stdadfx.h"

//
// Class representing a single instance of a socket connection file
//
class Socket {
public:

	//
	// <Method>
	//		Constructor
	// <Summary>
	//		Read the data from the Socket.
	//
	// @param  The Data retrieved from the server
	//
	Socket(int socketfd, struct sockaddr_in& address);

	//
	// Method:    Destructor
	//
	~Socket();

	//
	// <Method>
	//		Read
	// <Summary>
	//		Read the data from the Socket.
	// @param  The Data retrieved from the server
	//
	int read(char * &data);

	//
	// <Method>
	//		Send data
	// <Summary>
	//		Send the data to the socket
	// @param  data to send
	//
	int send(const char * &data);

	//
	// <Method>
	//		Close
	// <Summary>
	//		Close the socket
	//
  void close();

  //
  // <Method>
  //		Validate
  // <Summary>
  //		Check the Socket is correct.
  //
  boolean validate() {
    return socketFileDesc > -1; 
  }

private:
	//The Socket file descriptor
	int socketFileDesc;
	//Struct of SocAddr_in.
	sockaddr_in socketAddr;

};