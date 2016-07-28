#include <iostream>
#include "stdadfx.h"
#include "Socket.h"
#include "ServerSocket.h"
#include "ServerManager.h"
#include "AudioServerHandler.h"
#include "AudioClientManagerFactory.h"
#include "WavFilePackager.h"
#include <iostream>
#include <fstream>

int main(int, char**)
{


	cout << "Server Started \n";
  shared_ptr<ServerSocket> servSocket = make_shared<ServerSocket>("localhost", 29054);
  shared_ptr<ClientManagerFactory> factManager = make_shared<AudioClientManagerFactory>();
	
  ServerManager manager(servSocket, factManager);
	manager.listen();

  while (1) {}
}
