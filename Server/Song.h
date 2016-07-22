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

  Song() {}

  Song(const Song& song) {
    strcpy_s(SongName, song.SongName);
    strcpy_s(ArtistName, song.ArtistName);
    SongLength = song.SongLength;
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

