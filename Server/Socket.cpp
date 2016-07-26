#include "Socket.h"
#include "InputHandler.h"

//
// <Method>
//		Constructor
// <Summary>
//	Read the data from the Socket.
//
// @param  The Data retrieved from the server
//
Socket::Socket(int socket, struct sockaddr_in& address) {
	socketFileDesc = socket;
	socketAddr = address;
}

//
// <Method>
//		Destructor
// <Summary>
//		Close
//
Socket::~Socket() {
  cout << "Ending Socket" << endl;
	close();
}

//
// <Method>
//		Read
// <Summary>
//		Read the data from the Socket.
// @param  The size of the data from the server.
//
char* Socket::read(size_t& bytesRead) {
	bool ok = true;
  char* buffer = nullptr;
  //Read a buffer. 
  char intermediateBuf[2048];
	bytesRead = recv(socketFileDesc, intermediateBuf, 2048, 0);

	if (bytesRead <= 0) {
		ok = false;
    bytesRead = 0;
	}

	if (ok) {
    buffer = new char[bytesRead];
    memcpy(buffer, intermediateBuf, bytesRead);
	}

	return buffer;
}

//
// <Method>
//		Send data
// <Summary>
//		Send the data to the socket
// @param  data to send
//
int Socket::send(byte* data, size_t dataSize) {
	int returnVal = -1;
	if (data) {
		int returnVal = ::send(socketFileDesc, reinterpret_cast<char*>(data), dataSize, 0);
	}

	return returnVal;
}

//
// <Method>
//		Close
// <Summary>
//		Close the socket
//
void Socket::close() {}