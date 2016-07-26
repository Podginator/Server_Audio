#pragma once

#include "stdadfx.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "InputHandler.h"
#include <vector>
#include "ConcurrentQueue.h"
#include <atomic>

//
// The Manager Class to handle a single instance of 
// of a Socket
//
class ClientManager {

public:

  //
  // <Method>
  //		Constructor
  // <Summary>
  //		Constructs the Server Manager.
  // @param serPtr the Server Socket we want to listen on.
  //
  ClientManager(shared_ptr<Socket> socket);

  //
  // <Method>
  //		Constructor
  // <Summary>
  //		Constructs the Server Manager.
  // @param serPtr the Server Socket we want to listen on.
  // @param queue The Queue to drain from. 
  //
  ClientManager(shared_ptr<Socket> socket,
    shared_ptr<ConcurrentQueue<shared_ptr<Packet>>> queue);


  //Destructor.
  ~ClientManager();

  //
  // <Method>
  //		Start
  // <Summary>
  //		Starts the threads.
  //
  void ClientManager::start();

  //
  // <Method>
  //		addListener
  // <Summary>
  //		Adds a Listener to handle incoming responses.
  //@param handler the Handler we wish to add.
  void addListener(const shared_ptr<InputHandler>& handler);


  //
  // <Method>
  //		getHandlers
  // <Summary>
  //		Get all the handlers
  //@return The Input Handlers.
  vector<shared_ptr<InputHandler>> getHandlers();

  //
  // <Method>
  //		getSendQueue
  // <Summary>
  //		Get the send queue
  //@return The Send Queue.
  weak_ptr<ConcurrentQueue<shared_ptr<Packet>>> getSendQueue();

  //
  // <Method>
  //		CloseCleitn
  // <Summary>
  //		Close the Client.
  //    Joins the Threads. 
  //    Sets the boolean.
  //    Close the socket.
  //@return The Send Queue.
  void closeClient();

private:

  //The receive thread
  thread receiveThread; 

  //The Send Thread.
  thread sendThread;

  // The internal socket
  shared_ptr<Socket> mSocket;

  // An Input Handler.
  vector<shared_ptr<InputHandler>> mInputHandlers;

  // A Send Queue. We drain this queue and send back to the C++.
  shared_ptr<ConcurrentQueue<shared_ptr<Packet>>> mSendQueue;

  // The mutex
  mutex mMutex;

  // Have we acknowledged.
  atomic_bool mHasAcknowledged;

  //The Condition variable has events.
  condition_variable mAcknowledged;

  //We are running.
  atomic_bool mIsRunning = false; 

  //
  // <Method>
  //		sendTask
  // <Summary>
  //		Drains the Queue and sends any 
  //    packets to the connected client.
  void sendTask();

  //
  // <Method>
  //		recvTask
  // <Summary>
  //		Receive any incoming messages.
  //    Sends them to the input handlers for 
  //    Processing.
  void recvTask();
};