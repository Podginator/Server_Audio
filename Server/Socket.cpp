#include "Socket.h"
#include "InputHandler.h"

//
// <Method>
//    Constructor
// <Summary>
//  Read the data from the Socket.
//
// @param  The Data retrieved from the server
//
Socket::Socket(int socket, struct sockaddr_in& address) {
  socketFileDesc = socket;
  socketAddr = address;
}

//
// <Method>
//    Destructor
// <Summary>
//    Close
//
Socket::~Socket() {
  cout << "Ending Socket" << endl;
  close();
}

//
// <Method>
//    Read
// <Summary>
//    Read the data from the Socket.
// @param  The size of the data from the server.
//

size_t Socket::read(char* buffer, const size_t& bytesRead) {
  bool ok = true;  
  size_t res = 0;
  int retrieved = recv(socketFileDesc, buffer, bytesRead, 0);

  if (retrieved > 0) {
    res = static_cast<size_t>(retrieved);
  }

  //Error checking, if res = 0 close? 

  return res;
}

//
// <Method>
//    Send data
// <Summary>
//    Send the data to the socket
// @param  data to send
//
int Socket::send(byte* data, size_t dataSize) {
  int returnVal = -1;

  if (data) {
    int returnVal = ::send(socketFileDesc, reinterpret_cast<char*>(data), static_cast<int>(dataSize), 0);
  }

  return returnVal;
}

//
// <Method>
//    Close
// <Summary>
//    Close the socket
//
void Socket::close() {}