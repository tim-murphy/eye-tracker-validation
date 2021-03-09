// Measured data storage written to a file
// This is essentially a stream class wrapper with a few helpers
// Written by Tim Murphy <tim@murphy.org> 2021

#include "MeasuredDataFile.h"

#include <iostream>
#include <fstream>

MeasuredDataFile::MeasuredDataFile(const std::string &label,
                                   const std::string &trackerName,
                                   const std::string &subject,
                                   const std::string &filePath)
    : MeasuredDataStream(label, trackerName, subject, std::cout), // cout as placeholder
      filePath(filePath)
{
    outFile.open(filePath, std::ios::out | std::ios::app);

    if (!outFile.is_open())
    {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    finalOutStream = &outFile;
}

MeasuredDataFile::~MeasuredDataFile()
{
    outFile.close();
}

void MeasuredDataFile::writeBuffer()
{
    std::cout << "Writing data to " << filePath << " ... " << std::flush;

    // write the generated data file
    *finalOutStream << outStream.str() << std::flush;

    // and reset the stream
    std::stringstream().swap(outStream);

    std::cout << "done" << std::endl;
}