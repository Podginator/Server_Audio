#pragma once

#include "ClientManagerFactory.h"

// An Implementation of a Socket Manager Factory. This creates a Client Manager that 
// Responds to incoming Audio Calls. We pass the Drainable Queue to the Audio Server Handler. 
class AudioClientManagerFactory : public ClientManagerFactory {
public:
  // Create an Audio Client Manager. 
  virtual unique_ptr<ClientManager> createClientManager(unique_ptr<Socket> socket);
};
