// Measured data storage written to a stream
// Tim Murphy <tim@murphy.org> 2021

#include "MeasuredDataStream.h"

#include <ctime>
#include <iomanip>
#include <iostream>

MeasuredDataStream::MeasuredDataStream(const std::string &label,
                                       const std::string &trackerName,
                                       const std::string &subject,
                                       std::ostream &str)
    : MeasuredData(label, trackerName, subject), finalOutStream(&str)
{
    // CSV header
    outStream << "\"Label\",\"Subject\",\"Tracker\",\"Timestamp\",\"Target-ID\","
              << "\"Target-X\",\"Target-Y\","
              << "\"Cursor-X\",\"Cursor-Y\","
              << "\"Actual-X\",\"Actual-Y\""
              << std::endl;
}

void MeasuredDataStream::writeBuffer()
{
    // write the generated data file
    *finalOutStream << std::endl
        << "========= data output - copy/paste into notepad =========" << std::endl
        << outStream.str()
        << "=========================================================" << std::endl
        << std::endl;

    // and reset the stream
    std::stringstream().swap(outStream);
}

// A comma-delimited output to console.
// This can be used for testing, or piped into another process to
// avoid messing around with databases.
bool MeasuredDataStream::writeData(
    std::chrono::time_point<std::chrono::system_clock> timestamp,
    unsigned int targetNumber,
    unsigned int xTarget, unsigned int yTarget,
    unsigned int xCursor, unsigned int yCursor,
    unsigned int xActual, unsigned int yActual)
{
    // format the timestamp
    std::time_t ts = std::chrono::system_clock::to_time_t(timestamp);
    struct tm buffer;
#ifdef _WIN32
    localtime_s(&buffer, &ts);
#else
    localtime_r(&ts, &buffer);
#endif

    outStream << "\"" << getLabel() << "\","
              << "\"" << getSubject() << "\","
              << "\"" << getTrackerName() << "\","
              << "\"" << std::put_time(&buffer, "%F %T") << "\","
              << targetNumber << ","
              << xTarget << "," << yTarget << ","
              << xCursor << "," << yCursor << ","
              << xActual << "," << yActual << std::endl;

    return true;
}
