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
ServerManager::ServerManager(std::shared_ptr<ServerSocket>& serPtr,
  std::shared_ptr<SocketManagerFactory> socketFactory) {
  mManagerFactory = socketFactory;
  mServerSocket = serPtr;
  mServerSocket->begin();

}

//Destructor.
ServerManager::~ServerManager() {
}

//
// <Method>
//		removeSocket
// <Summary>
//   remove the reference to the socket from the list.
//
void ServerManager::removeSocket(std::shared_ptr<ClientManager> socket) {
  //To do remove.
  //Hold on to my past. 
  //It's been so long I don't think I'm gonna last. 
  socket->closeClient();
  
  //Remove the socket
  bool removed = mSocketSet.erase(socket);

  if (removed) {
    std::cout << "Removed Client" << std::endl;
  }
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
  std::thread thread = std::thread(&ServerManager::getConnections, this);
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
void ServerManager::acceptConnection(std::shared_ptr<Socket> socket) {
  std::shared_ptr<ClientManager> clientManager = mManagerFactory->createSocketManager(socket, this);
  mSocketSet.insert(clientManager);
  clientManager->start();
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
    std::shared_ptr<Socket> socket = mServerSocket->acceptSocket();
    if ((socket != nullptr) && (socket->validate())) {
      std::cout << "Connected! \n";
      acceptConnection(socket);
    }
  }
}
