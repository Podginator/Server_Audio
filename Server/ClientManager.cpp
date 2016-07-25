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
  mSendQueue = std::make_shared<ConcurrentQueue<Packet>>();
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

  receiveThread.join();


  // Send a closing flag. 
  std::shared_ptr<Packet> packet = std::make_shared<Packet>(Type::CLOSE, 0, nullptr);
  //Stop The Concurrent Queue from blocking to end.
  mSendQueue->push(Packet(Type::NO_OPP, 0, nullptr));

  sendThread.join();


  mInputHandlers.clear();
  closeClient();
}


//Destructor.
ClientManager::~ClientManager() {
  std::cout << "Deleted Client Manager" << std::endl; 
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
  mIsRunning = false; 

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
    std::unique_lock<std::mutex> drainLock(mMutex);
    auto item = mSendQueue->pop();
    //mAcknowledged.wait(drainLock, [&] { return mHasAcknowledged.load(); });
    
    mHasAcknowledged.store(false);
    //Send the response to the server.
    if ((item.packetData != nullptr && item.type != Type::NO_OPP)) {
      
      //Convert Packet down to byte array. 
      size_t packetSize = sizeof(Packet);
      byte* data = new byte[packetSize];
      
      // Convert a Packet down to a char* buffer and 
      // Send to the client
      memcpy(data, &item, packetSize);

      
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
      Packet packet = extractPacket(msg, msgSize);

      //If the packet we've retrieved is a no-operation
      //Then we have a correct packet, or received junk.
      if (packet.type != Type::NO_OPP) {
        // If we've received an acknowledge, then 
        // Alert the send thread that we can continue.
        if (packet.type == Type::ACKNOWLEDGE) {
          mHasAcknowledged.store(true);
          mAcknowledged.notify_all();
        }
        else {
          // Otherwise just iterate through the handles
          // That listen for this kind of packet.
          for each (auto handler in mInputHandlers) {
            if (handler->listensFor(packet.type)) {
              handler->handlePacket(packet);
            }
          }
        }
      }
    } else if (msgSize == 0) {
      mIsRunning.store(false);
    }

    delete[] msg;
  }

}
