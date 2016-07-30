#pragma once

#include "stdadfx.h"

//
// Class representing a single instance of a socket connection file
//
class Socket {
public:

  // Constructor: 
  // Tales a spclet 
  Socket(SOCKET socketfd, struct sockaddr_in& mAddress);

  //
  // Method:    Destructor
  //
  ~Socket();

  //Copy constructor.
  // Ensure that we cannot copy this class elsewhere, as it is 
  // a noncopyable resource (A socket.)
  Socket(const Socket& other) = delete;

  //Move Constructor 
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  Socket(Socket&& other) = default;

  //Move Operator.
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  Socket& operator=(Socket&&) & = default;

  //
  // <Method>
  //    Read
  // <Summary>
  //    Read the data from the Socket.
  // @param  The Data retrieved from the server
  //
  size_t read(char* buffer, const size_t& dataSize);

  //
  // <Method>
  //    Send data
  // <Summary>
  //    Send the data to the socket
  // @param  data to send
  //
  int send(byte* data, size_t dataSize);

  //
  // <Method>
  //    Close
  // <Summary>
  //    Close the socket
  //
  void close();

  //
  // <Method>
  //    Validate
  // <Summary>
  //    Check the Socket is correct.
  //
  boolean validate() {
    return mSocketFileDesc != 0; 
  }

private:
  //The Socket file descriptor
  SOCKET mSocketFileDesc;
  //Struct of SocAddr_in.
  sockaddr_in socketAddr;

};