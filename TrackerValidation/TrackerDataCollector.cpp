// collect normalised data from a gaze tracker
// Written by Tim Murphy <tim@murphy.org> 2021

#include "TrackerDataCollector.h"

// trackers
#include "DummyTrackerCollector.h"
#include "GazepointGP3Collector.h"

#include <stdexcept>

TrackerDataCollector::TrackerDataCollector(ScreenPositionStore &store)
    : position(store), processRunning(false)
{ }

bool TrackerDataCollector::isRunning() const
{
    return processRunning;
}

TrackerDataCollector *TrackerDataCollector::create(
    const std::string &tracker, ScreenPositionStore &store)
{
    if (tracker == "dummy")
    {
        return new DummyTrackerCollector(store);
    }
    else if (tracker == "GP3" || tracker == "gp3")
    {
        return new GazepointGP3Collector(store);
    }
    else
    {
        std::string err("Unknown tracker" + tracker);
        throw std::runtime_error(err.c_str());
    }
}
