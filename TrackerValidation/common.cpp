// common functionality and constants used within the project
// Written by Tim Murphy <tim@murphy.org> 2021

#include "common.h"

#include <iostream>

// for getScreenRes
#ifdef _WIN32
    #include <wtypes.h>
#else
    #include <X11/Xlib.h>
#endif

template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p)
{
    os << "pair(" << p.first << "," << p.second << ")";
    return os;
}

// forward declaration for unsigned int
template std::ostream &operator<<(
    std::ostream &, const std::pair<unsigned int, unsigned int> &);

std::pair<unsigned int, unsigned int> common::getScreenRes()
{
    std::pair<unsigned int, unsigned int> screenRes;

#ifdef _WIN32
    RECT screen;

    if (!GetWindowRect(GetDesktopWindow(), &screen))
    {
        throw std::runtime_error("Could not get screen resolution");
    }

    screenRes = std::make_pair(screen.right, screen.bottom);
#else
    Display *d = XOpenDisplay(nullptr);
    Screen *s = nullptr;

    if (d != nullptr)
    {
        s = DefaultScreenOfDisplay(d);
    }

    if (d == nullptr || s == nullptr)
    {
        throw std::runtime_error("Could not get screen resolution");
    }

    screenRes = std::make_pair(s->width, s->height);
#endif

    return screenRes;
}
