#pragma once

#include <map>
#include <stdio.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <vector>
#include "FileConverter.h"
#include <unordered_map>

template <class T> 
class FileList {
public: 

  //Constructor
  FileList(const string& fileLoc, const string& fileExtn, shared_ptr<FileConverter<T>> fileConvert) {
    mFileExtn = fileExtn;
    mFileLoc = fileLoc;
    fileConverter = fileConvert;
    populate();
  }

  //Return a vector of the Types. 
  vector<pair<T, string>> get() {
    return mFileMap;
  }

  //get File Name Ie The actual files name.
  string getFileName(int i) {
    if (i > mFileMap.size()) {
      // Throw
    }
    
    string fileName = nullptr;
    string path = getFullFilePath(i);

    if (path != nullptr) {
      size_t found = path.find_last_of("\\");
      if (found != string::npos) {
        //Then remove extn.
        string fullFileName = fileName.substr(0, found);
        found = fullFileName.find_last_of('.');
        fileName = fullFileName.substr(0, found);
      }
    }

    return fileName;
  }
  
  // Get the file 
  pair<T, string> operator[](int i) {
    return mFileMap.at(i);
  }

 

  //Get the Full File Path at this index. 
  string getFullFilePath(int i) {
    if (i > mFileMap.size()) {
      // Throw
    }

    return filePair.second();
  }

  // Get index of the T
  size_t indexOf(const T& obj) {
    size_t res, i = -1; 
    for (pair<T, string> objects : mFileMap) {
      i++; 
      if (objects.first == obj) {
        res = i;
        break;
      }
    }

    return res;
  }

  // Get the Directory we are looking in
  string getDirectory() {
    return mFileLoc;
  }

  // Return the File Extension
  string getExtension() {
    return mFileExtn;
  }

  // Create the File List.
  int populate() {
    string filePath = mFileLoc + "/*"+ mFileExtn;
    WIN32_FIND_DATA fileDir;
    HANDLE hFind = ::FindFirstFile(filePath.c_str(), &fileDir);

    if (hFind != INVALID_HANDLE_VALUE) {
      
      do {
        if (!(fileDir.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          string fileName(fileDir.cFileName);
          T file = fileConverter->getClassFromFile(mFileLoc, fileName);
          mFileMap.push_back(pair<T, string>( file, mFileLoc + fileName));
        }
      } while (::FindNextFile(hFind, &fileDir));
      
      ::FindClose(hFind);
    }

    return 1;
  }
  
  // Clear 
  void clear() {
    mFileMap.clear();
  }

private:


  // Location of the Files we're looking for 
  string mFileLoc;

  // Pointer to a file converter, takes the location of a file and 
  // converts it to the correct object. 
  shared_ptr<FileConverter<T>> fileConverter;

  // The Extension of the file. 
  string mFileExtn; 

  // The Vector of OBject and their locations
  vector<pair<T, string>> mFileMap;

};