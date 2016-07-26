#pragma once

template <class T> 
class FileConverter {
public :
  virtual T getClassFromFile(const string& fileDir, const string& fileName) = 0;
};