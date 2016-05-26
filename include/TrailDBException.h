#ifndef CPP_TRAILDBEXCEPTION_H_INCLUDED
#define CPP_TRAILDBEXCEPTION_H_INCLUDED

#include <exception>

class TrailDBException: public std::exception {
  public:
    virtual const char* what() //const throw() 
    {
      return "TrailDBcpp exception";
    }
};


#endif  //CPP_TRAILDBEXCEPTION_H_INCLUDED
