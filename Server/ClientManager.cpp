#pragma once

#include "ClientManager.h"
#include "PacketExtracter.h"

//
// <Method>
//		Constructor
// <Summary>
//		Constructs the Server Manager.
// @param serPtr the Server Socket we want to listen on.
//
ClientManager::ClientManager(std::shared_ptr<Socket> socket) : ClientManager(socket, std::make_shared<ConcurrentQueue<Packet>>()) {}

//
// <Method>
//		Constructor
// <Summary>
//		Constructs the Server Manager.
// @param serPtr the Server Socket we want to listen on.
// @param queue The Queue to drain from. 
//
ClientManager::ClientManager(std::shared_ptr<Socket> socket,
  std::shared_ptr<ConcurrentQueue<Packet>> queue) {
  mSocket = socket;
  mSendQueue = queue;
}

//
// <Method>
//		Start
// <Summary>
//		Starts the threads.
//
void ClientManager::start() {
  mIsRunning = true;

  receiveThread = std::thread(&ClientManager::recvTask, this);
  sendThread = std::thread(&ClientManager::sendTask, this);
}


//Destructor.
ClientManager::~ClientManager() {}

//
// <Method>
//		addListener
// <Summary>
//		Adds a Listener to handle incoming responses.
//@param handler the Handler we wish to add.
void ClientManager::addListener(const std::shared_ptr<InputHandler>& handler) {
  mInputHandlers.push_back(handler);
}


//
// <Method>
//		getHandlers
// <Summary>
//		Get all the handlers
//@return The Input Handlers.
std::vector<std::shared_ptr<InputHandler>> ClientManager::getHandlers() {
  return mInputHandlers;
}

//
// <Method>
//		getSendQueue
// <Summary>
//		Get the send queue
//@return The Send Queue.
std::shared_ptr<ConcurrentQueue<Packet>> ClientManager::getSendQueue() {
  return mSendQueue;
}


//
// <Method>
//		CloseCleitn
// <Summary>
//		Close the Client.
//    Joins the Threads. 
//    Sets the boolean.
//    Close the socket.
//@return The Send Queue.
void ClientManager::closeClient() {
  sendThread.join();
  receiveThread.join();

  //Wait to get to here, then close.
  mSocket->close();
}


//
// <Method>
//		sendTask
// <Summary>
//		Drains the Queue and sends any 
//    packets to the connected client.
void ClientManager::sendTask() {
  while (mIsRunning) {
    std::unique_lock<std::mutex> drainLock(mMutex, std::defer_lock);
    drainLock.lock();
    auto item = mSendQueue->pop();


    //Send the response to the server.
    if ((item.packetData != nullptr || item.type != Type::NO_OPP)) {
      mSocket->send((const char*)item.packetData, item.size);
    }

    drainLock.unlock();
  }
}

//
// <Method>
//		recvTask
// <Summary>
//		Receive any incoming messages.
//    Sends them to the input handlers for 
//    Processing.
void ClientManager::recvTask() {
  while (mIsRunning) {
    char* msg = nullptr;
    size_t msgSize = mSocket->read(msg);
    if (msgSize > 0 && msg != nullptr) {
      Packet packet = extractPacket(msg, msgSize);
      for each (auto handler in mInputHandlers) {
        handler->handlePacket(packet);
      }
    } else if (msgSize == 0) {
      // Send a closing flag. 
      Packet packet;
      packet.type = Type::CLOSE;
      for each (auto handler in mInputHandlers) {
        handler->handlePacket(packet);
      }

      mIsRunning = false;
      closeClient();
      
    }
  }
}
