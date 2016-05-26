#ifndef BYTE_MANIPULATIONS_H_INCLUDED
#define BYTE_MANIPULATIONS_H_INCLUDED

/*
 ByteManipulations.h
 Various helper functions for byte manipulations

 @author: Asif Imran
 @version: 0.1 12/16/2015
*/

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iomanip>

template <typename T>
const std::uint8_t* ParseValue(const std::uint8_t* start, T& data, size_t size) {
  const std::uint8_t* end = start + size;
  std::copy(start, end, reinterpret_cast<std::uint8_t*>(&data));
  return end;
}


#endif //BYTE_MANIPULATIONS_H_INCLUDED
