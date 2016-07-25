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
ClientManager::ClientManager(std::shared_ptr<Socket> socket) {
  mSocket = socket;
  mSendQueue = std::make_shared<ConcurrentQueue<std::shared_ptr<Packet>>>();
}

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
  sendThread = std::thread(&ClientManager::sendTask, this);
}


//Destructor.
ClientManager::~ClientManager() {
  std::cout << "Deleted Client Manager" << std::endl; 
  closeClient();
}

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

  if (receiveThread.joinable()) {
    receiveThread.join();
  }


  // Send a closing flag. 
  std::shared_ptr<Packet> packet = std::make_shared<Packet>(Type::CLOSE, 0, nullptr);

  //Stop The Concurrent Queue from blocking to end.
  mSendQueue->push(std::make_shared<Packet>(Type::NO_OPP, 0, nullptr));
  if (mIsRunning) {
    for each (auto handler in mInputHandlers) {
      if (handler->listensFor(packet->type)) {
        handler->handlePacket(packet);
      }
    }
  }
  
  if (sendThread.joinable()) {
    sendThread.join();
  }

  if (mSendQueue != nullptr) {
    //Empty the queue, removing any pending Sends. 
    for (int i = 0; i < mSendQueue->size(); i++) {
      // Pop this reference - Then delete it.
      mSendQueue->pop();
    }
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
    auto item = mSendQueue->pop();
    mAcknowledged.wait(drainLock, [&] { return (bool)mHasAcknowledged; });

    //Send the response to the server.
    if ((item->packetData != nullptr && item->type != Type::NO_OPP)) {
      
      //Convert Packet down to byte array. 
      size_t packetSize = sizeof(Packet);
      byte* data = new byte[packetSize];
      
      // Convert a Packet down to a char* buffer and 
      // Send to the client
      memcpy(data, item.get(), packetSize);

      mHasAcknowledged = false;
      //Why.
      mSocket->send(data, packetSize);
      delete[] data;


      
    }
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

      if (packet->type == Type::ACKNOWLEDGE) {
        //We have acknowledged. 
        mHasAcknowledged = true;
        mAcknowledged.notify_all();

      } else {
        for each (auto handler in mInputHandlers) {
          if (handler->listensFor(packet->type)) {
            handler->handlePacket(packet);
          }
        }
      }
      delete[] msg;
    }
    else if (msgSize == 0) {
      delete[] msg;
      closeClient();
    }
  }
}
