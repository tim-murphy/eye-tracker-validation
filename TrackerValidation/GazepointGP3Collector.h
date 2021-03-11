// Tracker data collector for the Gazepoint GP3 system.
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef GAZEPOINTGP3COLLECTOR_H
#define GAZEPOINTGP3COLLECTOR_H

#include "ThreadTrackerCollector.h"

#include <string>

class GazepointGP3Collector
    : public ThreadTrackerCollector
{
  private:
    void collectData();

  public:
    GazepointGP3Collector(ScreenPositionStore &store,
                          const TrackerConfig &config)
        : ThreadTrackerCollector(store, config)
    {}

    const std::string &getName() const
    {
        static const std::string name = "Gazepoint GP3";
        return name;
    }
};

# endif // defined GAZEPOINTGP3COLLECTOR_H
