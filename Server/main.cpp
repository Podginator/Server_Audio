#include <iostream>
#include "stdadfx.h"
#include "Socket.h"
#include "ServerSocket.h"
#include "ServerManager.h"
#include "AudioServerHandler.h"
#include "AudioSocketManagerFactory.h"
#include "WavFilePackager.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(int, char**)
{


	std::cout << "Server Started \n";
  std::shared_ptr<ServerSocket> servSocket = std::make_shared<ServerSocket>("localhost", 29054);
  std::shared_ptr<SocketManagerFactory> factManager = std::make_shared<AudioSocketManagerFactory>();
	
  ServerManager manager(servSocket, factManager);
	manager.listen();




  while (1) {}
}
