#include "AudioServerHandler.h"
#include "WavFilePackager.h"
#include <thread>

AudioServerHandler::AudioServerHandler(std::shared_ptr<ConcurrentQueue<std::shared_ptr<Packet>>> conQue) : InputHandler(Type::AUDIO | Type::TRACK | Type::EXIT | Type::FILELIST) {
    mConQueue = conQue;
    auto conv = std::make_shared<SongFileConverter>(SongFileConverter());
    fileList = std::make_shared<FileList<Song>>("C:\\", "wav", conv);
}


//
// <Method> 
//  getResponse
// <Summary> 
//  return a Response Struct to send back.
// @param sentMessage the Message we have been set.
void AudioServerHandler::handlePacket(const std::shared_ptr<Packet>& sentMessage) {
  //Change track, therefore. 
  switch (sentMessage->type) {
  case Type::AUDIO:
  case Type::TRACK: 
  {
    std::thread audioParser(&AudioServerHandler::requestFile, this, std::string("first"));
    audioParser.detach();
    break;
  }
  case Type::EXIT: {
    isRunning = false;
    break;
  }
  case Type::FILELIST:
    size_t songSize = sizeof(Song);
    std::vector<std::pair<Song, std::string>> songList = fileList->get();
    size_t maxBufferSize = songSize * songList.size();

    byte* byteArray = new byte[Packet::maxPacketSize];
    byte* pointer = byteArray;
    size_t used = 0;

    for (auto file : fileList->get()) {
      auto type = file.first;
      //Todo, buffer out into smaller packets. 
      if ((used + songSize) > Packet::maxPacketSize) {
        //Make the packet. 
        std::shared_ptr<Packet> packet = std::make_shared<Packet>(Type::FILELIST, used, byteArray);
        mConQueue->push(packet);

        //Clear buffer. 
        delete[] byteArray; 
        byteArray = new byte[Packet::maxPacketSize];
        pointer = byteArray;
        used = 0;
      }
      
      memcpy(pointer, &type, songSize);
      pointer += songSize;
      used += songSize;
    }


    std::shared_ptr<Packet> packet = std::make_shared<Packet>(Type::FILELIST, used, byteArray);
    mConQueue->push(packet);

    //Clear buffer. 
    delete[] byteArray;

    break;
  }
}



// Request the file and use it to 
void AudioServerHandler::requestFile(const std::string& fileName){
  //Wait for the response from the other thread. 
  
 
  if (isRunning || readThreadActive) {
    isRunning = false;

    std::unique_lock<std::mutex> lck(mMutex);
    //Then wait until we have confirmation that all things have shut down. Countdown latch equivalent?
    mIsOnly.wait(mMutex, [&] {return !readThreadActive; });
  }

  isRunning = true;
  readThreadActive = true;

  //First grab the file
  WavFilePackager packager;
  packager.openFile("C:\\daisy.wav");

  // Check that we can 
  if (packager.isFileOpen() && isRunning) {

    size_t headerSize = 44;
    byte* headerBuffer = new byte[headerSize];
    packager.getHeader(headerBuffer, headerSize);

    std::shared_ptr<Packet> packet = std::make_shared<Packet>(Type::AUDIO, headerSize, headerBuffer);
    mConQueue->push(packet);

    while (!packager.isFileComplete() && isRunning) {
      size_t bufferSize = Packet::maxPacketSize;
      byte* dataBuffer = new byte[bufferSize];
      
      size_t actualSize = packager.getNextChunk(dataBuffer, bufferSize);
      std::shared_ptr<Packet> dataPacket = std::make_shared<Packet>(Type::AUDIO, actualSize, dataBuffer);
      
      mConQueue->push(packet);
    }
  }
 
  isRunning = false;
  readThreadActive = false;

  mIsOnly.notify_one();
}