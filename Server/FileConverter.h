#pragma once

template <class T> 
class FileConverter {
public :
  virtual T getClassFromFile(const std::string& fileDir, const std::string& fileName) = 0;
};