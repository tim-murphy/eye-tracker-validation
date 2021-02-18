// Class for manual storage of screen (x,y) pixel coordinates.
// The source of this data could be a TCP stream, FIFO, file, etc.
// This is defined as a "store" as it refers to the last known value.
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef SCREENPOSITIONSTORE_H
#define SCREENPOSITIONSTORE_H

#include <iosfwd> // for operator<<
#include <mutex>
#include <utility> // for std::pair

class ScreenPositionStore
{
  protected:
    // Current position (x, y) screen co-ordinates, in pixels
    std::pair<unsigned int, unsigned int> currentPos;

    // Some trackers will give an identifier (sequence number, time, etc.).
    // If no identifier is given, it will default to zero.
    double identifier;

    // position information is written by one thread and retrieved by another.
    std::mutex posMutex;

  public:
    ScreenPositionStore(std::pair<unsigned int, unsigned int> pos = std::make_pair(0,0),
                        double id = 0.0);

    // Retrieve the stored position.
    // Note that this is only thread safe if called within a lock() / unlock()
    // block. Locking hasn't been done within this method as we may want to
    // call getIdentifier() or do some other action within the same lock.
    std::pair<unsigned int, unsigned int> getCurrentPosition() const;

    // Retrieve the stored identifier.
    // This will return 0.0 if no identifier has even been set.
    double getIdentifier() const;

    // set the position with (x, y) co-ordinates
    void setCurrentPosition(std::pair<unsigned int, unsigned int> pos,
                            double id = 0.0);

    friend std::ostream &operator<<(std::ostream &os,
                                    const ScreenPositionStore &store);

    // mutex operations. We may want to perform multiple actions in a locked
    // state, so we'll allow callers to lock/unlock where necessary.
    void lock(void);
    void unlock(void);
};

#endif // not defined SCREENPOSITIONSTORE_H
