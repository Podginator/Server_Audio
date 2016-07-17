#pragma once

#include "FileConverter.h"
#include "Song.h"
#include "WavFilePackager.h"
#include "stdadfx.h"

class SongFileConverter : public FileConverter<Song> {
public: 

  SongFileConverter() {
    packager = WavFilePackager();
  }

  virtual Song getClassFromFile(const std::string& fileDir, const std::string& fileName) {
    WavFilePackager::waveHeader header;
    size_t bufferSize = 44;
    char* buffer = new char[bufferSize];
    Song res;

    packager.openFile(fileDir + '\\' + fileName);
    if (packager.isFileOpen()) {
      packager.getHeader(buffer, bufferSize);
      memcpy(&header, buffer, bufferSize);

      int timeLen = packager.getTotalSize() / (header.sample_rate * header.num_channels * header.bits_per_sample / 8);
      std::size_t found = fileName.find("_");
      if (found != std::string::npos) {
        //eh. Do this later.
        std::string songName = fileName.substr(0, found);
        std::string artistName = fileName.substr(found);

        strcpy_s(res.SongName, songName.size() + 1, songName.c_str());
        strcpy_s(res.ArtistName, artistName.size() + 1, artistName.c_str());
      }
      else {
        strcpy_s(res.SongName, fileName.size() + 1, fileName.c_str());
        strcpy_s(res.ArtistName, 8, "Unknown");
      }

      res.SongLength = timeLen;
    }

    return res;
  }


private:
  WavFilePackager packager;
};