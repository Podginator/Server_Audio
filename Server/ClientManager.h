#pragma once

#include "stdadfx.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "InputHandler.h"
#include <vector>
#include "ConcurrentQueue.h"

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
  ClientManager(std::shared_ptr<Socket> socket);

  //
  // <Method>
  //		Constructor
  // <Summary>
  //		Constructs the Server Manager.
  // @param serPtr the Server Socket we want to listen on.
  // @param queue The Queue to drain from. 
  //
  ClientManager(std::shared_ptr<Socket> socket,
    std::shared_ptr<ConcurrentQueue<Packet>> queue);


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
  void addListener(const std::shared_ptr<InputHandler>& handler);


  //
  // <Method>
  //		getHandlers
  // <Summary>
  //		Get all the handlers
  //@return The Input Handlers.
  std::vector<std::shared_ptr<InputHandler>> getHandlers();

  //
  // <Method>
  //		getSendQueue
  // <Summary>
  //		Get the send queue
  //@return The Send Queue.
  std::shared_ptr<ConcurrentQueue<Packet>> getSendQueue();

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
  std::thread receiveThread; 

  //The Send Thread.
  std::thread sendThread;

  // The internal socket
  std::shared_ptr<Socket> mSocket;

  // An Input Handler.
  std::vector<std::shared_ptr<InputHandler>> mInputHandlers;

  // A Send Queue. We drain this queue and send back to the C++.
  std::shared_ptr<ConcurrentQueue<Packet>> mSendQueue;

  // The mutex
  std::mutex mMutex;

  //The Condition variable has events.
  std::condition_variable mHasItems;

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