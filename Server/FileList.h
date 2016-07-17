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
  FileList(const std::string& fileLoc, const std::string& fileExtn, std::shared_ptr<FileConverter<T>> fileConvert) {
    mFileExtn = fileExtn;
    mFileLoc = fileLoc;
    fileConverter = fileConvert;
    populate();
  }

  //Return a vector of the Types. 
  std::vector<T> get() {
    return std::vector<T>;
  }

  //get File Name 
  std::string getFileName(const T& fileType) {
    return "none";
  }

  std::string getDirectory() {
    return "C:\\Hey";
  }

  std::string getExtension() {
    return "etx";
  }

  FILE* getFile(const T& object) {
    return nullptr;
  }

  // Create the File List.
  int populate() {
    std::string filePath = mFileLoc + "/*"+ mFileExtn;
    WIN32_FIND_DATA fileDir;
    HANDLE hFind = ::FindFirstFile(filePath.c_str(), &fileDir);

    if (hFind != INVALID_HANDLE_VALUE) {
      
      do {
        if (!(fileDir.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          std::string fileName(fileDir.cFileName);
          T file = fileConverter->getClassFromFile(mFileLoc, fileName);
          mFileMap.push_back(std::pair<T, std::string>( file, mFileLoc + fileName));
        }
      } while (::FindNextFile(hFind, &fileDir));
      
      ::FindClose(hFind);
    }

    return 1;
  }
  
  // Clear 
  void clear() {

  }


private:


  std::string mFileLoc;

  std::shared_ptr<FileConverter<T>> fileConverter;


  std::string mFileExtn; 

  std::vector<std::pair<T, std::string>> mFileMap;

};