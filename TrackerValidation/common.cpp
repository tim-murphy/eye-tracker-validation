// common functionality and constants used within the project
// Written by Tim Murphy <tim@murphy.org> 2021

#include "common.h"

#include <iostream>

template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p)
{
    os << "pair(" << p.first << "," << p.second << ")";
    return os;
}

// forward declaration for unsigned int
template std::ostream &operator<<(
    std::ostream &, const std::pair<unsigned int, unsigned int> &);
