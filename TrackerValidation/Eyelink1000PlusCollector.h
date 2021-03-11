// Tracker data collector for the SR Eyelink 1000 Plus system.
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef EYELINK1000PLUSCOLLECTOR_H
#define EYELINK1000PLUSCOLLECTOR_H

#include "ThreadTrackerCollector.h"

class Eyelink1000PlusCollector
    : public ThreadTrackerCollector
{
  private:
    void collectData();

  public:
    Eyelink1000PlusCollector(ScreenPositionStore &store,
                             const TrackerConfig &config)
        : ThreadTrackerCollector(store, config)
    {}

    const std::string &getName() const
    {
        static const std::string name = "SR Eyelink 1000 Plus";
        return name;
    }
};

#endif // not defined EYELINK1000PLUSCOLLECTOR_H