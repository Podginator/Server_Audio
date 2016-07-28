#include "ServerSocket.h"
#include <errno.h>

// <Method>
//    Constructor
// <Summary>
//    Constructs the Server Objects
// @param hostName the host name to connect to (Ie: IP-Port)
// @param port the port to listen to
//
ServerSocket::ServerSocket(const string& hostName, int port) {
  mPort = port;
  address = hostName;
}

//Destructor.
ServerSocket::~ServerSocket() {}

// <Method>
//  accept
// <Summary>
//   Accept a connection that is attempting to connection
//   Return a unique pointer to the Socket that is created.
// @return Pointer to a Socket.
//
unique_ptr<Socket> ServerSocket::acceptSocket() {
  struct sockaddr_in cliAddr;
  socklen_t cliLen = sizeof(cliAddr);
  SOCKET s =
    accept(socketFileDesc, (sockaddr*)&cliAddr, &cliLen);

  if (s != SOCKET_ERROR) {
    printf("Client connected to the Server. ");
    return make_unique<Socket>(s, cliAddr);
  }

  return nullptr;
}

//
// <Method>
//   Begin Listening
// <Summary>
//   Starts listening on the Socket.
// @return True if successful, false otherwise.
//
bool ServerSocket::begin() {
  bool ok = true;
  struct addrinfo *result = NULL;
  struct addrinfo hints;
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;

  /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
  wVersionRequested = MAKEWORD(2, 2);
  err = WSAStartup(wVersionRequested, &wsaData);

  if (err != 0) {
    ok = false; 
  }

  if (ok) {
    //TODO TCP OR UDP Or what? 
    socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    //Check to see if we've failed.
    if (socketFileDesc < 0) {
      ok = false;
    }

    if (ok) {
      setupServerHints(hints);
      if (getaddrinfo(address.c_str(), to_string(mPort).c_str(), &hints, &result)) {
        ok = false;
      }
    }

    if (ok) {

      if (::bind(socketFileDesc, result->ai_addr, (int)result->ai_addrlen) < 0) {
        ok = false;
      }

      freeaddrinfo(result);
    }
  }

  //If we've successfully set up our server, then listen
  //on it, otherwise close.
  if (ok) {
    int res = listen(socketFileDesc, SOMAXCONN);

    if (res == SOCKET_ERROR) {
      ok = false;
    }
  }

  if (!ok) {
    close();
  }

  return ok;
}

// 
// <Method>
//  Close
// <Summary> 
//  Close the socket.
//
void ServerSocket::close() {
  int errNum = WSAGetLastError();

  if (errNum != 0) {
    cout << "Server Socket closed due to error " << errNum << endl;
  }

  WSACleanup();

  if (socketFileDesc >= 0) {
    //::close(socketFileDesc);
    socketFileDesc = -1;
  }
}

// <Method>
//    setupServerAddressStruct
// <Summary>
//    Sets up pthe Sockaddr_in 
// @param ref The Socket_addr_in to modify.
//
void ServerSocket::setupServerHints(addrinfo& hints) {
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
}
