// common functionality and constants used within the project
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef COMMON_H
#define COMMON_H

// for operator<< for std::pair
#include <iosfwd>
#include <utility>

namespace common
{

// pi constant
constexpr double pi = 3.14159265358979323846264338327950288419716939937510582097494459230781640629;

// get the resolution of the current screen
std::pair<unsigned int, unsigned int> getScreenRes();

} // end namespace common

// operator overloads need to live out of the namespace
template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p);

#endif // not defined COMMON_H
