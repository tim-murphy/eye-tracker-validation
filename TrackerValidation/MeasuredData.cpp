// Abstract class for storage of measured data.
// Tim Murphy <tim@murphy.org> 2021

#include "MeasuredData.h"

MeasuredData::MeasuredData(std::string trackerName)
    : name(trackerName)
{}

MeasuredData::~MeasuredData()
{
    // ensure any buffered data is written
    writeBuffer();
}

const std::string &MeasuredData::getName() const
{
    return name;
}

void MeasuredData::writeBuffer()
{ }