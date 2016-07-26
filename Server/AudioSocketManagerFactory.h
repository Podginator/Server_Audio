#pragma once

#include "SocketManagerFactory.h"

// An Implementation of a Socket Manager Factory. This creates a Client Manager that 
// Responds to incoming Audio Calls. We pass the Drainable Queue to the Audio Server Handler. 
class AudioSocketManagerFactory : public SocketManagerFactory {
public:
  virtual shared_ptr<ClientManager> createSocketManager(shared_ptr<Socket> socket, ServerManager* servMan) {
    //Make an instance of the Client Handler.
    shared_ptr<ClientManager> audioManager = make_shared<ClientManager>(socket);

    //Get the drain queue from the audio Manager
    weak_ptr<ConcurrentQueue<Packet>> drainQueue = audioManager->getSendQueue();

    //Create an audio handler. 
    shared_ptr<InputHandler> audioHandler = make_shared<AudioServerHandler>(drainQueue);
    //Create the ClosingServerHandler
    //shared_ptr<InputHandler> closeHandler = make_shared<CloserServerHandler>(servMan, audioManager);
    
    //add the listeners.
    audioManager->addListener(audioHandler);
    //audioManager->addListener(closeHandler);

    socket = nullptr;

    return audioManager;
  }

};
