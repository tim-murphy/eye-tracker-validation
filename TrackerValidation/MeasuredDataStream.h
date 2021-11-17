// Measured data storage written to a stream
// Tim Murphy <tim@murphy.org> 2021

#ifndef MEASUREDDATASTREAM_H
#define MEASUREDDATASTREAM_H

#include "MeasuredData.h"

#include <iosfwd>
#include <sstream>

class MeasuredDataStream : public MeasuredData
{
  protected:
    // this stream is used to build the output data
    std::stringstream outStream;

    // this is where the final data is written
    std::ostream *finalOutStream;

  public:
    MeasuredDataStream(const std::string &label,
                       const std::string &trackerName,
                       const std::string &subject,
                       std::ostream &outstream);

    bool writeData(
        std::chrono::time_point<std::chrono::system_clock> timestamp,
        unsigned int targetNumber,
        unsigned int xTarget, unsigned int yTarget,
        unsigned int xCursor, unsigned int yCursor,
        unsigned int xActualRight, unsigned int yActualRight,
        unsigned int xActualLeft, unsigned int yActualLeft);

    virtual void writeBuffer();
};

#endif // defined MEASUREDDATASTREAM_H