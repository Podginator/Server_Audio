#include "AudioServerHandler.h"
#include "WavFilePackager.h"
#include <thread>

AudioServerHandler::AudioServerHandler(std::shared_ptr<ConcurrentQueue<Packet>> conQue) {
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
void AudioServerHandler::handlePacket(const Packet& sentMessage) {
  //Change track, therefore. 
  switch (sentMessage.type) {
  case Type::AUDIO:
  case Type::TRACK: {
    std::thread audioParser(&AudioServerHandler::requestFile, this, std::string("first"));
    audioParser.detach();
    break;
  }
  case Type::EXIT: {
    std::unique_lock<std::mutex> lk(mMutex);
    isRunning = false;
    lk.unlock();
    break;
  }
  case Type::FILELIST:
    size_t songSize = sizeof(Song);
    std::vector<std::pair<Song, std::string>> songList = fileList->get();
    size_t bufferSize = songSize * songList.size();

    char* byteArray = new char[bufferSize];
    char* pointer = byteArray;

    for (auto file : fileList->get()) {
      auto type = file.first;
      memcpy(pointer, &type, songSize);
      pointer += songSize;
    }

    Packet packet;
    packet.type = Type::FILELIST;
    packet.packetData = byteArray;
    packet.size = bufferSize;
    
    mConQueue->push(packet);

    break;
  }
}



// Request the file and use it to 
void AudioServerHandler::requestFile(const std::string& fileName){
  //Wait for the response from the other thread. 
  std::unique_lock<std::mutex> lk(mMutex);

  //Ensure thhat we are no longer running. 
  isRunning = !isRunning;

  //Then wait until we have confirmation that all things have shut down. Countdown latch equivalent?
  mIsOnly.wait(lk, [&] {return !readThreadActive; });

  //Get Track from file Then Decipher it. For now use the one file we have. 
  isRunning = true;
  readThreadActive = true;

  lk.unlock();

  //First grab the file
  WavFilePackager packager;
  packager.openFile("C:\\daisy.wav");

  // Check that we can 
  if (packager.isFileOpen() && isRunning) {
    char* headerBuffer = new char[44];
    size_t headerSize = 44;
    packager.getHeader(headerBuffer, headerSize);

    Packet packet;
    packet.type = Type::AUDIO;
    packet.size = headerSize;
    packet.packetData = headerBuffer;
    mConQueue->push(packet);
    while (!packager.isFileComplete() && isRunning) {
      size_t bufferSize = 350;
      char* dataBuffer = new char[bufferSize];
      Packet dataPacket;
      dataPacket.type = Type::AUDIO;
      dataPacket.size = bufferSize;
      packager.getNextChunk(dataBuffer, bufferSize);
      dataPacket.packetData = dataBuffer;
      mConQueue->push(packet);
    }
  }

  // Lock around is running, and set false. 
  std::unique_lock<std::mutex> lock(mMutex);
  isRunning = false;
  readThreadActive = false;
  lock.unlock();
  mIsOnly.notify_one();
}