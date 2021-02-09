// Tracker Data Collector which collects data in a separate thread.
// This will be most of the collectors.
// Simply write the logic in a collectData() method and you're good to go.
// Written by Tim Murphy <tim@murphy.org> 2021

#include "ThreadTrackerCollector.h"

ThreadTrackerCollector::~ThreadTrackerCollector()
{
    stop();
}

void ThreadTrackerCollector::run()
{
    if (isRunning())
    {
        // process is running already - leave it be
        return;
    }

    processRunning = true;

    collectionThread = std::thread(&ThreadTrackerCollector::collectData, this);
}

void ThreadTrackerCollector::stop()
{
    if (!isRunning())
    {
        // not running - nothing to do
        return;
    }

    processRunning = false;

    collectionThread.join();
}