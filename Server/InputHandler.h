#pragma once


enum Type {
  NO_OPP = 0,
  ACKNOWLEDGE,
  EXIT,
  AUDIO,
  FILELIST,
  TRACK,
  CLOSE,
  FRIEND
};

/**
* The response object.
*/
struct Packet {
public:

  //Type of the Packet. 
  Type type;

  // Size of the Data. 
  size_t size;

  //Char* Array.
  const char* packetData;

};

/**
* The Server Handler Interface.
*/
class InputHandler
{
public:
  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
	virtual void handlePacket(const Packet& packet) = 0;
};