// A test gaze tracker data collector.
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef DUMMYTRACKERCOLLECTOR_H
#define DUMMYTRACKERCOLLECTOR_H

#include "ThreadTrackerCollector.h"

#include "ScreenPositionStore.h"

#include <string>

class DummyTrackerCollector
    : public ThreadTrackerCollector
{
  private:
    void collectData();

  public:
    DummyTrackerCollector(ScreenPositionStore &store,
                          const TrackerConfig &config);

    const std::string &getName() const
    {
        static const std::string name = "Mouse";
        return name;
    }
};

#endif // defined DUMMYTRACKERCOLLECTOR_H
