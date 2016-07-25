#include "AudioServerHandler.h"
#include "WavFilePackager.h"
#include <iostream>
#include <fstream>


//Constructor. 
// Pass the Song File Converter to the File List. 
// Pass the queue.
AudioServerHandler::AudioServerHandler(std::weak_ptr<ConcurrentQueue<Packet>> conQue) : InputHandler(Type::AUDIO | Type::TRACK | Type::EXIT | Type::FILELIST) {
    mConQueue = conQue;
    auto conv = std::make_shared<SongFileConverter>();
    fileList = std::make_shared<FileList<Song>>("C:\\", "wav", conv);
}

//
// <Method> 
//  getResponse
// <Summary> 
//  return a Response Struct to send back.
// @param sentMessage the Message we have been set.
void AudioServerHandler::handlePacket(const Packet& sentMessage) {
  //Change track, therefore. 
  switch (sentMessage.type) {
  case Type::TRACK: 
  {
    // If one is already running, join it.
    if (fileParserThread.joinable()) {
      isRunning = false; 
      fileParserThread.join();
    }

    std::shared_ptr<Song> packetSong = std::make_shared<Song>();
    

    int sizeOf = sizeof(Song);
    if (sizeOf == sentMessage.size) {
      memcpy(packetSong.get(), sentMessage.packetData, sentMessage.size);

        // Then Run a new thread requesting the file and sending it.
      fileParserThread = std::thread(&AudioServerHandler::requestFile, this, packetSong);
      fileParserThread.detach();
    }
    break;
  }
  case Type::EXIT: {
    isRunning = false;
    if (fileParserThread.joinable()) {
      fileParserThread.join();
    }
    break;
  }
  case Type::FILELIST:
    size_t songSize = sizeof(Song);
    size_t maxBufferSize = songSize * fileList->get().size();

    byte* byteArray = new byte[Packet::maxPacketSize];
    byte* pointer = byteArray;
    size_t used = 0;

    for (auto file : fileList->get()) {
      auto type = file.first;
      //Todo, buffer out into smaller packets. 
      if ((used + songSize) > Packet::maxPacketSize) {
        //Make the packet. 
        Packet intermediatePacket(Type::FILELIST, used, byteArray);

        if (auto queue = mConQueue.lock()) {
          queue->push(intermediatePacket);
        }

        //Delete the existing one.
        delete[] byteArray;
        //Clear buffer. 
        byteArray = new byte[Packet::maxPacketSize];
        pointer = byteArray;
        used = 0;
      }
      
      memcpy(pointer, &type, songSize);
      pointer += songSize;
      used += songSize;
    }


    Packet finalPacket(Type::FILELIST, used, byteArray);
    if (auto queue = mConQueue.lock()) {
      queue->push(finalPacket);
    }

    //Clear buffer. 
    delete[] byteArray;

    break;
  }
}

// Request the file and use it to 
void AudioServerHandler::requestFile(std::shared_ptr<Song> song){
  //Wait for the response from the other thread. 
  
  //Ensure that there's only 1 Read thread active in this instance. If not, attempt to kill it. 
  if (isRunning || readThreadActive) {
    isRunning = false;

    std::unique_lock<std::mutex> lck(mMutex);
    //Then wait until we have confirmation that all things have shut down. Countdown latch equivalent?
    mIsOnly.wait(mMutex, [&] {return !readThreadActive; });
  }

  isRunning = true;
  readThreadActive = true;

  //Get the index of the thread.
  int index = fileList->indexOf(*song);

  //If we've found the file. 
  if (index > -1) {

    std::string fileName = fileList->get().at(index).second;

    //First grab the file
    WavFilePackager packager;
    packager.openFile(fileName);

    // Check that we can 
    if (packager.isFileOpen() && isRunning) {

      size_t headerSize = 44;
      byte* headerBuffer = new byte[headerSize];
      packager.getHeader(headerBuffer, headerSize);
      Packet packet(Type::AUDIO, headerSize, headerBuffer);
      delete[] headerBuffer;

      if (auto queue = mConQueue.lock()) {
        queue->push(packet);
      }

      while (!packager.isFileComplete() && isRunning) {
        size_t bufferSize = Packet::maxPacketSize;
        byte* dataBuffer = new byte[bufferSize];

        size_t actualSize = packager.getNextChunk(dataBuffer, bufferSize);
        Packet dataPacket(Type::AUDIO, actualSize, dataBuffer);

        if (auto queue = mConQueue.lock()) {
          queue->push(dataPacket);
        }

        delete[] dataBuffer;
      }
    }
    packager.closeFile();
  }

 
  isRunning = false;
  readThreadActive = false;

  mIsOnly.notify_one();
}