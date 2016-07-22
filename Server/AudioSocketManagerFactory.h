#pragma once

#include "SocketManagerFactory.h"

// An Implementation of a Socket Manager Factory. This creates a Client Manager that 
// Responds to incoming Audio Calls. We pass the Drainable Queue to the Audio Server Handler. 
class AudioSocketManagerFactory : public SocketManagerFactory {
public:
  virtual std::shared_ptr<ClientManager> createSocketManager(std::shared_ptr<Socket>& socket, ServerManager* servMan) {
    //Make an instance of the Client Handler.
    std::shared_ptr<ClientManager> audioManager = std::make_shared<ClientManager>(socket);

    //Get the drain queue from the audio Manager
    std::shared_ptr<ConcurrentQueue<std::shared_ptr<Packet>>> drainQueue = audioManager->getSendQueue();

    //Create an audio handler. 
    std::shared_ptr<InputHandler> audioHandler = std::make_shared<AudioServerHandler>(drainQueue);
    //Create the ClosingServerHandler
    std::shared_ptr<InputHandler> closeHandler = std::make_shared<CloserServerHandler>(servMan, audioManager);
    
    //add the listeners.
    audioManager->addListener(audioHandler);
    audioManager->addListener(closeHandler);

    return audioManager;
  }

};
