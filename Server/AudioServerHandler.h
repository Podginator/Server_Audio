#pragma once
#include "InputHandler.h"
#include "ConcurrentQueue.h"
#include "FilePackager.h"
#include "WavFilePackager.h"
#include "InputHandler.h"
#include "FileList.h"
#include "Song.h"
#include "SongFileConverter.h"
#include <atomic>


/**
* The AudioServer Handler
*/
class AudioServerHandler : public InputHandler {
public:

  //Constructor, takes a ConcurrentQueue, which is often associated with 
  //the 
  AudioServerHandler(std::shared_ptr<ConcurrentQueue<std::shared_ptr<Packet>>> conQue);
 
  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
  void handlePacket(const std::shared_ptr<Packet>& sentMessage);
private: 
  
  // Is the current thread running
  std::atomic_bool isRunning = false;

  // Is the read thread running
  std::atomic_bool readThreadActive = false;

  std::mutex mMutex;

  // Is this the only thread running.
  std::condition_variable_any mIsOnly;
  
  //The Concurrent Queue to drain from. 
  std::shared_ptr<ConcurrentQueue<std::shared_ptr<Packet>>> mConQueue;

  std::shared_ptr<FileList<Song>> fileList;

  //Request a file. 
  void requestFile(const std::string& fileName);
};