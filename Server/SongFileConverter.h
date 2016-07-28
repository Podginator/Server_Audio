#pragma once

#include "FileConverter.h"
#include "Song.h"
#include "WavFilePackager.h"
#include "stdadfx.h"

// The File Converter
// From File To Song.
class SongFileConverter : public FileConverter<Song> {
public: 

  // Constructor. Takes a Wav File Pacakger to enable 
  // Us to get the header from the Wav File.
  SongFileConverter() {
    packager = WavFilePackager();
  }

  // Returns the Class from the file. 
  virtual Song getClassFromFile(const string& fileDir, const string& fileName) {
    WavFilePackager::waveHeader header;
    size_t bufferSize = 44;
    byte* buffer = new byte[bufferSize];
    Song res;

    packager.openFile(fileDir + '\\' + fileName);
    if (packager.isFileOpen()) {
      packager.getHeader(buffer, bufferSize);
      memcpy(&header, buffer, bufferSize);
      delete[] buffer;

      //int timeLen = packager.getTotalSize() / (header.sample_rate * header.num_channels * header.bits_per_sample / 8);
      int timeLen = 400;
      size_t found = fileName.find("_");
      if (found != string::npos) {
        //eh. Do this later.
        string songName = fileName.substr(0, found);
        string artistName = fileName.substr(found);

        strcpy_s(res.SongName, songName.size() + 1, songName.c_str());
        strcpy_s(res.ArtistName, artistName.size() + 1, artistName.c_str());
      }
      else {
        strcpy_s(res.SongName, fileName.size() + 1, fileName.c_str());
        strcpy_s(res.ArtistName, 8, "Unknown");
      }

      res.SongLength = timeLen;
    }
    packager.closeFile();

    return res;
  }


private:
  // The Pacakger
  WavFilePackager packager;
};