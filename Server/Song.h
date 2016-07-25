#pragma once

// Simple Model class that stores:
// Song Name
// Artist Name
// Song Length. 
struct Song {

  //Song Name
  char SongName[64]; 

  //Artists name
  char ArtistName[64];

  // The length of the song in seconds.
  int SongLength; 

  Song() {
  
  }

  ~Song() {}

  Song(const Song &obj) {
    //Copy Song and Artist Names. 
    strncpy_s(SongName, obj.SongName, 64);
    strncpy_s(ArtistName, obj.ArtistName, 64);
    SongLength = obj.SongLength;
  }

  Song & operator= (const Song &obj) {
    //Copy Song and Artist Names. 
    strncpy_s(SongName, obj.SongName, 64);
    strncpy_s(ArtistName, obj.ArtistName, 64);

    SongLength = obj.SongLength;
  }

  //Check equality. 
  bool operator ==(const Song &b) const {
    //Equal if all inner components are equal
    if (strcmp(SongName, b.SongName) != 0) {
      return false; 
    }

    if (strcmp(ArtistName, b.ArtistName) != 0) {
      return false;
    }

    if (SongLength != b.SongLength) {
      return false; 
    }

    return true;
  }
};

