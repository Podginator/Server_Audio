#pragma once

/**
* The response object.
*/
struct Packet {
public:

  enum TYPE {
    NO_OPP = 0,
    ACKNOWLEDGE,
    EXIT,
    AUDIO,
    FILE,
    TRACK,
    FRIEND
  };

	TYPE responseId;

	const char* response;
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