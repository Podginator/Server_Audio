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

using namespace std;

/**
* The AudioServer Handler
*/
class AudioServerHandler : public InputHandler {
public:

  //Constructor, takes a ConcurrentQueue, which is often associated with 
  //the 
  AudioServerHandler(weak_ptr<ConcurrentQueue<shared_ptr<Packet>>> conQue);
 
  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
  void handlePacket(const weak_ptr<Packet>& sentMessage);
private: 
  
  // Is the current thread running
  atomic_bool isRunning = false;

  // Is the read thread running
  atomic_bool readThreadActive = false;
  
  // The Mutex.
  mutex mMutex;

  // Is this the only thread running.
  condition_variable_any mIsOnly;
  
  //The Concurrent Queue to drain from. 
  weak_ptr<ConcurrentQueue<shared_ptr<Packet>>> mConQueue;

  // The File List of songs.
  shared_ptr<FileList<Song>> fileList;

  // The File Parser thread. 
  thread fileParserThread;


  //Request a file. 
  void requestFile(shared_ptr<Song> fileName);
};