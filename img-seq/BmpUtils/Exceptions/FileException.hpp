#pragma once
#include <exception>

using namespace std;
 //TODO explain in slides
class FileException: public exception{
  virtual const char* what() const throw(){
    return "Error writing files";
  }
};