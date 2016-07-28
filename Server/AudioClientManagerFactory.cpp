#pragma once

#include "AudioClientManagerFactory.h"
#include "AudioServerHandler.h"


// Create the Client Manager. Pass in the correct Socket.  
unique_ptr<ClientManager> AudioClientManagerFactory::createClientManager(unique_ptr<Socket> socket) {
   //Make an instance of the Client Handler.
   unique_ptr<ClientManager> audioManager(new ClientManager(std::move(socket)));

   //Get the drain queue from the audio Manager
   weak_ptr<ConcurrentQueue<Packet>> drainQueue = audioManager->getSendQueue();

   //Create an audio handler. 
   shared_ptr<InputHandler> audioHandler = make_shared<AudioServerHandler>(drainQueue);

   //add the listeners.
   audioManager->addListener(audioHandler);

   return audioManager;
  }