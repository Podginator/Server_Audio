#pragma once

// The File Converter, Takes a File Path and a name, and attempts to convert it 
// To the Template.
template <class T> 
class FileConverter {
public :
  // Get the class from the file name.
  virtual T getClassFromFile(const string& fileDir, const string& fileName) = 0;
};