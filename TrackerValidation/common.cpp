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
    HMONITOR activeMon = MonitorFromWindow(GetActiveWindow(),
                                           MONITOR_DEFAULTTONEAREST);
    MONITORINFO monInfo;
    monInfo.cbSize = sizeof(monInfo);
    if (GetMonitorInfo(activeMon, &monInfo) == 0)
    {
        throw std::runtime_error("Could not get screen resolution");
    }

    unsigned int horiz = (monInfo.rcMonitor.right - monInfo.rcMonitor.left);
    unsigned int vert = (monInfo.rcMonitor.bottom - monInfo.rcMonitor.top);

    screenRes = std::make_pair(horiz, vert);
#else
    Display *d = XOpenDisplay(nullptr);
    Screen *s = nullptr;

    if (d != nullptr)
    {
        s = DefaultScreenOfDisplay(d);
    }

    if (d == nullptr || s == nullptr)
    {
        if (d != nullptr)
        {
            XCloseDisplay(d);
        }

        throw std::runtime_error("Could not get screen resolution");
    }

    screenRes = std::make_pair(s->width, s->height);

    XCloseDisplay(d);
#endif

    return screenRes;
}
