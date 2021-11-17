// A test gaze tracker data collector.
// Written by Tim Murphy <tim@murphy.org> 2021

#include "DummyTrackerCollector.h"

#include <chrono>

#ifdef _WIN32
    #include <Windows.h>
    #include <WinUser.h>
#else
    #include <X11/Xlib.h>

    typedef struct POINT {
        unsigned int x;
        unsigned int y;
    } POINT;
    bool GetCursorPos(POINT *pt)
    {
        Window root, child;
        int screenX, screenY, windowX, windowY;
        unsigned int mask;
        Display *disp = XOpenDisplay(nullptr);
        if (XQueryPointer(disp, DefaultRootWindow(disp), &root, &child,
            &screenX, &screenY, &windowX, &windowY, &mask))
        {
            pt->x = screenX;
            pt->y = screenY;
        }
        else
        {
            // could not determine mouse pointer
            pt->x = 0;
            pt->y = 0;
        }
        XCloseDisplay(disp);
        
        return true;
    }
#endif

DummyTrackerCollector::DummyTrackerCollector(ScreenPositionStore &store,
                                             const TrackerConfig &config)
    : ThreadTrackerCollector(store, config)
{ }

void DummyTrackerCollector::collectData()
{
    // keep looping until we are told to stop
    double sequence = 0;
    while (isRunning())
    {
        POINT pt;
        if (GetCursorPos(&pt))
        {
            unsigned int x = pt.x, y = pt.y;
            position.setCurrentPositionSingle(std::make_pair(x, y), ++sequence);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}
