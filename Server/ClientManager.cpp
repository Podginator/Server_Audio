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
ClientManager::ClientManager(std::shared_ptr<Socket> socket) : ClientManager(socket, std::make_shared<ConcurrentQueue<std::shared_ptr<Packet>>>()) {}

//
// <Method>
//		Constructor
// <Summary>
//		Constructs the Server Manager.
// @param serPtr the Server Socket we want to listen on.
// @param queue The Queue to drain from. 
//
ClientManager::ClientManager(std::shared_ptr<Socket> socket,
  std::shared_ptr<ConcurrentQueue<std::shared_ptr<Packet>>> queue) {
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
  receiveThread.detach();
  sendThread = std::thread(&ClientManager::sendTask, this);
  sendThread.detach();
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
std::shared_ptr<ConcurrentQueue<std::shared_ptr<Packet>>> ClientManager::getSendQueue() {
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
  mIsRunning = false; 

  //Send this to the queue to close.
  mSendQueue->push(std::make_shared<Packet>(Type::NO_OPP, 0, nullptr));

  //If these two threads are still running, ensure t hat we've killed them.
  if (sendThread.joinable()) {
    sendThread.join();
  }

  if (receiveThread.joinable()) {
    receiveThread.join();
  }

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
    if ((item->packetData != nullptr && item->type != Type::NO_OPP)) {
      
      //Convert Packet down to byte array. 
      size_t packetSize = sizeof(Packet);
      byte* data = new byte[packetSize];
      
      // Convert a Packet down to a char* buffer and 
      // Send to the client
      memcpy(data, item.get(), packetSize);

      mSocket->send(data, packetSize);

      //Then delete the character array. 
      delete[] data;
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
      std::shared_ptr<Packet> packet = extractPacket(msg, msgSize);
      for each (auto handler in mInputHandlers) {
        if (handler->listensFor(packet->type)) {
          handler->handlePacket(packet);
        }
      }
    } else if (msgSize == 0) {
      // Send a closing flag. 
      std::shared_ptr<Packet> packet = std::make_shared<Packet>(Type::CLOSE, 0, nullptr);
      packet->type = Type::CLOSE;

      //Stop The Concurrent Queue from blocking to end.
      mSendQueue->push(std::make_shared<Packet>(Type::NO_OPP, 0, nullptr));
      for each (auto handler in mInputHandlers) {
        if (handler->listensFor(packet->type)) {
          handler->handlePacket(packet);
        }
      }

      mIsRunning = false;
      closeClient();
    }
  }
}
