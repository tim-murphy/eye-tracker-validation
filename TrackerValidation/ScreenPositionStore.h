// Class for manual storage of screen (x,y) pixel coordinates.
// The source of this data could be a TCP stream, FIFO, file, etc.
// This is defined as a "store" as it refers to the last known value.
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef SCREENPOSITIONSTORE_H
#define SCREENPOSITIONSTORE_H

#include <iosfwd> // for operator<<
#include <utility> // for std::pair

class ScreenPositionStore
{
  protected:
    // Current position (x, y) screen co-ordinates, in pixels
    std::pair<unsigned int, unsigned int> currentPos;

    // Some trackers will give an identifier (sequence number, time, etc.).
    // If no identifier is given, it will default to zero.
    double identifier;

  public:
    // retrieve the stored values
    std::pair<unsigned int, unsigned int> getCurrentPosition() const;
    double getIdentifier() const;

    // set the position with (x, y) co-ordinates
    void setCurrentPosition(std::pair<unsigned int, unsigned int> pos,
                            double id = 0.0);

    friend std::ostream &operator<<(std::ostream &os,
                                    const ScreenPositionStore &store);
};

#endif // not defined SCREENPOSITIONSTORE_H
