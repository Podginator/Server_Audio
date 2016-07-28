#include "AudioServerHandler.h"
#include "WavFilePackager.h"
#include <iostream>
#include <fstream>


//Constructor
// conQue: Weak_Ptr to a ConcurrentQueue of Packets. This is where we'll send to.
// Tell the InputHandler that we deal with Track, Exit and File List.
AudioServerHandler::AudioServerHandler(weak_ptr<ConcurrentQueue<Packet>> conQue) 
  : InputHandler(Type::TRACK | Type::EXIT | Type::FILELIST) {
    mConQueue = conQue;
    fileList = make_shared<FileList<Song>>("C:\\", "wav", make_shared<SongFileConverter>());
}

//Handle the Packet we have sent 
//  sentMessage: The Packet we wish to process.  
void AudioServerHandler::handlePacket(const Packet& sentMessage) {

  switch (sentMessage.type) {
  case Type::TRACK:
  {
    //We are changing track, so attempt to end the currently parsing thread. 
    isRunning.store(false);
    
    //Cast the packet to a song.
    shared_ptr<Song> packetSong = make_shared<Song>();

    //Ensure that the packets contents are the size of a song (as a preliminary check)
    if (sentMessage.size == sizeof(Song)) {
      //Copy the memory of the song to a song object. 
      memcpy(packetSong.get(), sentMessage.packetData, sentMessage.size);
      //Then Run a new thread requesting the file and sending it.
      thread(&AudioServerHandler::requestFile, this, packetSong).detach();
    }
    break;
  }
  case Type::EXIT: {
    isRunning = false;
    break;
  }
  case Type::FILELIST:

    //Package off and send back.
    thread([=] {
      size_t songSize = sizeof(Song);
      size_t maxBufferSize = songSize * fileList->size();

      byte* byteArray = new byte[Packet::maxPacketSize];
      byte* pointer = byteArray;
      int used = 0;

      for (auto file : fileList->get()) {
        auto type = file.first;

        if ((used + songSize) > Packet::maxPacketSize) {

          if (auto queue = mConQueue.lock()) {
            //Make the packet. 
            Packet intermediatePacket(Type::FILELIST, used, byteArray);
            //Delete the existing one.
            delete[] byteArray;
            queue->push(intermediatePacket);
          }
          else {
            break;
          }

          //Clear buffer. 
          byteArray = new byte[Packet::maxPacketSize];
          pointer = byteArray;
          used = 0;
        }

        memcpy(pointer, &type, songSize);
        pointer += songSize;
        used += songSize;
      }

      if (auto queue = mConQueue.lock()) {
        Packet finalPacket(Type::FILELIST, used, byteArray);
        queue->push(finalPacket);
      }

      //Clear buffer. 
      delete[] byteArray;
    }).detach();


    break;
  }
 
}

// Requst a song, chop it up and send it to te mConQueue
//   fileName : The Song File we want to dissect.
void AudioServerHandler::requestFile(shared_ptr<Song> song){
  
  //Ensure that there's only 1 Read thread active in this instance. If not, attempt to kill it. 
  if (isRunning.load() || readThreadActive.load()) {
    isRunning.store(false);

    unique_lock<mutex> lck(mMutex);
    //Then wait until we have confirmation that all things have shut down. Countdown latch equivalent?
    mIsOnly.wait(mMutex, [&] {return !readThreadActive.load(); });
  }

  isRunning.store(true);
  readThreadActive.store(true);

  //Get the index of the thread.
  int index = fileList->indexOf(*song);

  //If we've found the file. 
  if (index > -1) {

    string fileName = fileList->get().at(index).second;

    //First grab the file
    WavFilePackager packager;
    packager.openFile(fileName);

    // Check that we can 
    if (packager.isFileOpen() && isRunning) {

      size_t headerSize = 44;
      byte* headerBuffer = new byte[headerSize];
      packager.getHeader(headerBuffer, headerSize);
      Packet packet(Type::AUDIO, (int) headerSize, headerBuffer);
      delete[] headerBuffer;

      if (auto queue = mConQueue.lock()) {
        queue->push(packet);
      }

      while (!packager.isFileComplete() && isRunning) {
        size_t bufferSize = Packet::maxPacketSize;
        byte* dataBuffer = new byte[bufferSize];

        size_t actualSize = packager.getNextChunk(dataBuffer, bufferSize);
        Packet dataPacket(Type::AUDIO, (int) actualSize, dataBuffer);

        if (auto queue = mConQueue.lock()) {
          queue->push(dataPacket);
        }

        delete[] dataBuffer;
      }
    }
    packager.closeFile();
  }

  isRunning.store(false);
  readThreadActive.store(false);

  //Notify all waiting threads. 
  mIsOnly.notify_all();
}