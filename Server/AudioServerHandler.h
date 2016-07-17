#pragma once
#include "InputHandler.h"
#include "ConcurrentQueue.h"
#include "FilePackager.h"
#include "WavFilePackager.h"
#include "InputHandler.h"
#include "FileList.h"
#include "Song.h"
#include "SongFileConverter.h"

/**
* The AudioServer Handler
*/
class AudioServerHandler : public InputHandler {
public:

  //Constructor, takes a ConcurrentQueue, which is often associated with 
  //the 
  AudioServerHandler(std::shared_ptr<ConcurrentQueue<Packet>> conQue);
  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
  void handlePacket(const Packet & sentMessage);
private: 
  bool isRunning = false;

  bool readThreadActive = false;


  // The Mutex 
  std::mutex mMutex;

  //
  std::condition_variable mIsOnly;
  //The Concurrent Queue to drain from. 
  std::shared_ptr<ConcurrentQueue<Packet>> mConQueue;

  std::shared_ptr<FileList<Song>> fileList;

  //Request a file. 
  void requestFile(const std::string& fileName);
};