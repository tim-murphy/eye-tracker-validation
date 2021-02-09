// A test gaze tracker data collector.
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef DUMMYTRACKERCOLLECTOR_H
#define DUMMYTRACKERCOLLECTOR_H

#include "ThreadTrackerCollector.h"

#include "ScreenPositionStore.h"

#include <thread>

class DummyTrackerCollector
    : public ThreadTrackerCollector
{
  private:
    void collectData();

  public:
    DummyTrackerCollector(ScreenPositionStore &store);
};

#endif // defined DUMMYTRACKERCOLLECTOR_H