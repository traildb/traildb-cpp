#ifndef CPP_TRAILDBEXCEPTION_H_INCLUDED
#define CPP_TRAILDBEXCEPTION_H_INCLUDED

#include <iostream>
#include <exception>
#include <string.h>

class TrailDBException: public std::exception {
  public:
    TrailDBException(const std::string msg = "TrailDB Exception occurred"):msg_(msg){}
    const char* what() const throw() 
    {
      return msg_.c_str();
    }
  private:
    std::string msg_;
};


#endif  //CPP_TRAILDBEXCEPTION_H_INCLUDED
