#pragma once

#include "ClientManager.h"
#include "PacketExtracter.h"

//
// Constructor
//  serPtr : The Server Socket we want to listen on.
ClientManager::ClientManager(unique_ptr<Socket> socket) {
  mSocket = std::move(socket);
  mSendQueue = make_shared<ConcurrentQueue<Packet>>();
}

//
// Constructor
//  serPtr : The Server Socket we want to listen on.
//  queue  : The Queue we are sending from.
ClientManager::ClientManager(unique_ptr<Socket> socket,
  shared_ptr<ConcurrentQueue<Packet>> queue) {
  mSocket = std::move(socket);
  mSendQueue = queue;
}

// Start 
//    Start the Client.
void ClientManager::start() {
  mIsRunning = true;
  mHasAcknowledged.store(true);

  receiveThread = thread(&ClientManager::recvTask, this);
  sendThread = thread(&ClientManager::sendTask, this);

  receiveThread.join();

  //Unblock the send queue, and then join.
  mSendQueue->push(Packet(Type::INVALID, 0, nullptr));
  sendThread.join();


  mInputHandlers.clear();
  closeClient();
}


//Destructor.
ClientManager::~ClientManager() {
  cout << "Deleted Client Manager" << endl; 
}

// Add a Listener to handle incoming responses.
//   handler : The handler we want to deal with 
void addListener(const shared_ptr<InputHandler>& handler);
void ClientManager::addListener(const shared_ptr<InputHandler>& handler) {
  mInputHandlers.push_back(handler);
}

// Return all the handlers associated with this Client.
vector<shared_ptr<InputHandler>> ClientManager::getHandlers() {
  return mInputHandlers;
}

// Returns the Send queue
weak_ptr<ConcurrentQueue<Packet>> ClientManager::getSendQueue() {
  return mSendQueue;
}


// Close this client.
void ClientManager::closeClient() {
  mIsRunning = false; 

  //Wait to get to here, then close.
  mSocket->close();
}


// Send Task.
//  Send the Packets in the queue.
void ClientManager::sendTask() {
  while (mIsRunning) {
    unique_lock<mutex> drainLock(mMutex);
    auto item = mSendQueue->pop();
    mAcknowledged.wait(drainLock, [&] { return mHasAcknowledged.load(); });
    
    mHasAcknowledged.store(false);
    //Send the response to the server.
    if ((item.type != Type::INVALID)) {
      
      //Convert Packet down to byte array. 
      size_t packetSize = sizeof(Packet);
      byte* data = new byte[packetSize];
      
      // Convert a Packet down to a char* buffer and 
      // Send to the client
      memcpy(data, &item, packetSize);

      mSocket->send(data, packetSize);
      delete[] data;
    }
  }
}

// Receive Task
//  Receive the Packets in the queue
void ClientManager::recvTask() {
  while (mIsRunning) {


    //Attempt to retrieve a packet.
    size_t msgSize = 0;
    size_t recvSize = sizeof(Packet);
    char* buffer = new char[recvSize];
    msgSize =  mSocket->read(buffer, recvSize);
    if (msgSize > 0 && buffer != nullptr) {
      Packet packet = extractPacket(buffer, msgSize);

      //If the packet we've retrieved is a no-operation
      //Then we have a correct packet, or received junk.
      if (packet.type != Type::INVALID) {
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

    delete[] buffer;
  }
}
