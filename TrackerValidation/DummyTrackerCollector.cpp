// A test gaze tracker data collector.
// Written by Tim Murphy <tim@murphy.org> 2021

#include "DummyTrackerCollector.h"

#include <chrono>

#ifdef _WIN32
    #include <Windows.h>
    #include <WinUser.h>
#else
    typedef struct POINT {
        unsigned int x;
        unsigned int y;
    } POINT;
    bool GetCursorPos(POINT *pt)
    {
        pt->x = 100;
        pt->y = 300;
        return true;
    }
#endif

DummyTrackerCollector::DummyTrackerCollector(ScreenPositionStore &store)
    : ThreadTrackerCollector(store)
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
            position.setCurrentPosition(std::make_pair(x, y), ++sequence);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}
