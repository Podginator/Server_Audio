#include "ServerManager.h"
#include "EventHandler.h"
#include "Event.h"
#include <exception>
//
// <Method>
//    Constructor
// <Summary>
//    Constructs the Server Manager.
// @param serPtr the Server Socket we want to listen on.
//
ServerManager::ServerManager(unique_ptr<ServerSocket>& serPtr,
  unique_ptr<ClientManagerFactory>& socketFactory) {
  mManagerFactory = move(socketFactory);
  mServerSocket = move(serPtr);
  // This 
  if (!mServerSocket->begin()) {
    throw exception("Cannot start server");
  }

}

//Destructor.
ServerManager::~ServerManager() {
}


//
// <Method>
//    listen
// <Summary>
//   Accept a connection that is attempting to connection
//   Return a unique pointer to the Socket that is created.
// @return Pointer to a Socket.
//
void ServerManager::listen() {
  //Listen as a daemon.
  thread thread = std::thread(&ServerManager::getConnections, this);
  thread.detach();
}

//
// <Method>
//    acceptConnection
// <Summary>
//   Accept a connection that is attempting to connection
//   Return a unique pointer to the Socket that is created.
// @return Pointer to a Socket.
//
void ServerManager::acceptConnection(unique_ptr<Socket> socket) {
  unique_ptr<ClientManager> clientManager = mManagerFactory->createClientManager(move(socket));
  clientManager->start();
}

//
// <Method>
//    getConnections
// <Summary>
//   Listen to the Server Socket, accepting any 
// @return Pointer to a Socket.
//
void ServerManager::getConnections() {
  while (true) {
    unique_ptr<Socket> socket = mServerSocket->acceptSocket();
    if ((socket != nullptr) && (socket->validate())) {
      cout << "Connected! \n";
      thread([&] { acceptConnection(move(socket)); }).detach();
    }

    int errNo = WSAGetLastError();
    if (errNo != 0 && errNo != WSAEINTR) {
      break;
    }

  }
}
