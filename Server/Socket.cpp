#include "Socket.h"

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
	close();
}

//
// <Method>
//		Read
// <Summary>
//		Read the data from the Socket.
// @param  The Data retrieved from the server
//
int Socket::read(char*& data) {
	bool ok = true;
	int totalBytes = 0;
  //Read a buffer. 
  char* buffer = new char[1024];
	int byteRead = recv(socketFileDesc, buffer, 2048, 0);

	if (byteRead <= 0) {
		ok = false;
		totalBytes = 0;
	}

	if (ok) {
    totalBytes = byteRead;
    data = new char[totalBytes + 1];
    memcpy(data, buffer, totalBytes);
    data[totalBytes] = '\0';
	}

  delete buffer;
	return totalBytes;
}

//
// <Method>
//		Send data
// <Summary>
//		Send the data to the socket
// @param  data to send
//
int Socket::send(const char*& data, size_t dataSize) {
	int returnVal = -1;
	if (data) {
		int returnVal = ::send(socketFileDesc, data, dataSize, 0);
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