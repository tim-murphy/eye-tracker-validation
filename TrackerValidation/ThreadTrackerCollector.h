// Tracker Data Collector which collects data in a separate thread.
// This will be most of the collectors.
// Simply write the logic in a collectData() method and you're good to go.
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef THREADTRACKERCOLLECTOR_H
#define THREADTRACKERCOLLECTOR_H

#include "TrackerDataCollector.h"

class ThreadTrackerCollector
    : public TrackerDataCollector
{
  private:
    // we will run the actual collection in a separate thread
    std::thread collectionThread;
    virtual void collectData() = 0;

  public:
    ThreadTrackerCollector(ScreenPositionStore &store,
                           const TrackerConfig &config)
        : TrackerDataCollector(store, config)
    {}

    virtual ~ThreadTrackerCollector();

    // start the collection thread
    void run();

    // stop the collection thread
    void stop();
};

#endif // defined THREADTRACKERCOLLECTOR_H
