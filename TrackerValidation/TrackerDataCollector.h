// collect normalised data from a gaze tracker
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef TRACKERDATACOLLECTOR_H
#define TRACKERDATACOLLECTOR_H

#include "ScreenPositionStore.h"

#include "TrackerConfig.h"

#include <string>
#include <thread>

// This uses a factory pattern to allow different trackers to be added to the
// system more easily.
class TrackerDataCollector
{
  protected:
    // storage object for the current screen position
    ScreenPositionStore &position;

    // tracker configuration data
    const TrackerConfig &config;

    // is the data collector process running?
    bool processRunning;

    // constructor hidden as this is using a factory pattern
    TrackerDataCollector(ScreenPositionStore& store,
                         const TrackerConfig &config);

  public:
    // create a data collector of the type defined in tracker.
    // throws std::runtime_error if tracker does not match any known type.
    static TrackerDataCollector *create(const std::string &tracker,
                                        ScreenPositionStore &store,
                                        const TrackerConfig &config);

    // Start the data collector process. If it is already running, this will
    // do nothing.
    virtual void run() = 0;

    // Stop the data collector process. If it is not running, this will do
    // nothing.
    virtual void stop() = 0;

    // The name of this tracker (overloaded by each implementing class).
    virtual const std::string &getName() const = 0;

    // Is the data collector process running?
    bool isRunning() const;
};

#endif // defined TRACKERDATACOLLECTOR_H
