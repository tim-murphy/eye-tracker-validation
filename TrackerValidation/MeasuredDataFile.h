// Measured data storage written to a file
// This is essentially a stream class wrapper with a few helpers
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef MEASUREDDATAFILE_H
#define MEASUREDDATAFILE_H

#include "MeasuredDataStream.h"

#include <fstream>
#include <string>

class MeasuredDataFile : public MeasuredDataStream
{
  private:
    std::ofstream outFile;
    std::string filePath;

  public:
    MeasuredDataFile(const std::string &label,
                     const std::string &trackerName,
                     const std::string &filePath);

    ~MeasuredDataFile();

    void writeBuffer();
};

#endif // defined MEASUREDDATAFILE_H