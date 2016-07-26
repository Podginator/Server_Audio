#include "ServerManager.h"
#include "EventHandler.h"
#include "Event.h"
//
// <Method>
//		Constructor
// <Summary>
//		Constructs the Server Manager.
// @param serPtr the Server Socket we want to listen on.
//
ServerManager::ServerManager(shared_ptr<ServerSocket>& serPtr,
  shared_ptr<SocketManagerFactory> socketFactory) {
  mManagerFactory = socketFactory;
  mServerSocket = serPtr;
  mServerSocket->begin();

}

//Destructor.
ServerManager::~ServerManager() {
}


//
// <Method>
//		listen
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
//		acceptConnection
// <Summary>
//   Accept a connection that is attempting to connection
//   Return a unique pointer to the Socket that is created.
// @return Pointer to a Socket.
//
void ServerManager::acceptConnection(shared_ptr<Socket> socket) {
  shared_ptr<ClientManager> clientManager = mManagerFactory->createSocketManager(socket, this);
  clientManager->start();
  socket = nullptr;
}

//
// <Method>
//		getConnections
// <Summary>
//   Listen to the Server Socket, accepting any 
// @return Pointer to a Socket.
//
void ServerManager::getConnections() {
  while (true) {
    shared_ptr<Socket> socket = mServerSocket->acceptSocket();
    if ((socket != nullptr) && (socket->validate())) {
      cout << "Connected! \n";
      thread([=] { acceptConnection(socket); }).detach();
    }
    socket = nullptr;
  }
}
