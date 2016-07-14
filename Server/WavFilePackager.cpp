#pragma once

#include "WavFilePackager.h"


//CTOR: Create an instance of a file with which we're wanting to disect. 
WavFilePackager::WavFilePackager() {
  mHeader = { 0 };
  mOpenFile = nullptr;
  mTotalSize = 0;
  mExtracted = 0;
}

//Name: isFileOpen
//  Is the file open
//Return: bool
//  True if file open.
bool WavFilePackager::isFileOpen() {
  return mOpenFile != nullptr;
}

//Name: isFileComplete
//  Is the file completely read.
//Return : Bool
//  True if file is complete.
bool WavFilePackager::isFileComplete() {
  return mExtracted >= mTotalSize;
}

//Name: Open File
//  Open a file
//Param: String : filePath
//  The Path to the file.
//Return Bool: 
//  The File has opened and been read correctly.
bool WavFilePackager::openFile(const std::string& filePath) {
  FILE * file;
  fopen_s(&file, filePath.c_str(), "r");
  //Then open the file.
  return openFile(file);
}

//Name: OpenFile
//  Open a file.
//Param: FILE*
//  The File we want to open.
//Return : Bool
//  Successfully opened a file.
bool WavFilePackager::openFile(FILE* file) {
  bool isOpen = false; 

  // Close the currently open file.
  if (mOpenFile) {
    closeFile();
  }

  // Reset the Size and the total extracted. 
  mExtracted = 0;
  mOpenFile = file;


  //Then process the wav file. 
  isOpen = processWavFile(file, mHeader, mTotalSize);
  mExtracted = sizeof(waveHeader);

  return isOpen;
}

//Name : Close file
//  Close the file
//Return:
//  Return whether the file has been closed.
bool WavFilePackager::closeFile() {
  bool closed = false;
  if (mOpenFile != nullptr) {
    // We have closed if the error is not 0.
    closed = fclose(mOpenFile) == 0;
  }

  return closed;
}

//Name: Get Next Chunk.
//  Get the next chunk of <bufferSize> of this file.
//Param : Char* Buffer.
//  The buffer we want to write to.
//Param : size_t bufferSize.
//  The Size of the buffer we want to write to.
//Return : Size_T
//   The size of the written buffer.
size_t WavFilePackager::getNextChunk(char* buffer, size_t bufferSize) {
  int err = 0;
  size_t res = 0;
  //Fseek from extracted location.
  err = fseek(mOpenFile, static_cast<long>(mExtracted), SEEK_SET);

  if (err == 0) {
    //Then extract from the file the next chunk. 
    res = fread(buffer, bufferSize, 1, mOpenFile);
    mExtracted += bufferSize;
  }

  //Return the size of the buffer.
  return res;
}

//Name: Get Header
//  Get the Header of this file.
//Param : Char* Buffer.
//  The buffer we want to write to.
//Param : size_t bufferSize.
//  The Size of the buffer we want to write to.
//Return : Size_T
//   The size of the written buffer.
size_t WavFilePackager::getHeader(char* buffer, size_t bufferSize) {
  size_t read = bufferSize >= sizeof(waveHeader) ? sizeof(waveHeader) : bufferSize;

  //Then copy the header into the buffer.
  memcpy(buffer, &mHeader, read);

  return read;
}

//Name: Process the Wav File
//  Process the wav file.
//Param : FILE* file.
//  The File we want to read;
//Param : WaveHeader wavHdr
//  Process the heaver
//Param :  Size_T size 
//   size of the buffer..
bool WavFilePackager::processWavFile(FILE* file, waveHeader& wavhdr, size_t& size) {
  bool ok = true;
  waveHeader header = { 0 };

  fread(header.chunk_id, sizeof(byte), 4, file);

  if (std::strncmp(reinterpret_cast<char*>(header.chunk_id), "RIFF", 4) == 0) {
    //So continue.
    //Read the chunksizes
    fread(&header.chunk_size, sizeof(DWORD), 1, file);
    fread(&header.format, sizeof(DWORD), 1, file);
    if (std::strncmp(reinterpret_cast<char*>(header.format), "WAVE", 4) == 0) {
      fread(&header.subchunk1_id, sizeof(DWORD), 1, file);
      fread(&header.subchunk1_size, sizeof(DWORD), 1, file);
      fread(&header.audio_format, sizeof(WORD), 1, file);
      fread(&header.num_channels, sizeof(WORD), 1, file);
      fread(&header.sample_rate, sizeof(byte), 4, file);
      fread(&header.byte_rate, sizeof(DWORD), 1, file);
      fread(&header.block_align, sizeof(WORD), 1, file);
      fread(&header.bits_per_sample, sizeof(WORD), 1, file);
      fread(&header.subchunk2_id, sizeof(byte), 4, file);
      fread(&header.subchunk2_size, sizeof(byte), 4, file);
    }
    else {
      ok = false;
    }
  }
  else {
    ok = false;
  }

  wavhdr = header;
  //The total size of the data in this WAV File. We will request it in chunks.
  size = header.subchunk2_size;

  return ok;
}
