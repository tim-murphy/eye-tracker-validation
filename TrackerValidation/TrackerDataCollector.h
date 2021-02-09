// collect normalised data from a gaze tracker
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef TRACKERDATACOLLECTOR_H
#define TRACKERDATACOLLECTOR_H

#include "ScreenPositionStore.h"

#include <string>
#include <thread>

// This uses a factory pattern to allow different trackers to be added to the
// system more easily.
class TrackerDataCollector
{
  protected:
    // storage object for the current screen position
    ScreenPositionStore &position;

    // is the data collector process running?
    bool processRunning;

    TrackerDataCollector(ScreenPositionStore& store);

  public:
    static TrackerDataCollector *create(const std::string &tracker,
                                        ScreenPositionStore &store);

    // Start the data collector process. If it is already running, this will
    // do nothing.
    virtual void run() = 0;

    // Stop the data collector process. If it is not running, this will do
    // nothing.
    virtual void stop() = 0;

    // Is the data collector process running?
    bool isRunning() const;
};

#endif // defined TRACKERDATACOLLECTOR_H