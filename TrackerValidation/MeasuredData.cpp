// Abstract class for storage of measured data.
// Tim Murphy <tim@murphy.org> 2021

#include "MeasuredData.h"

#include "MeasuredDataFile.h"
#include "MeasuredDataStream.h"

#include <iostream>
#include <stdexcept>

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

MeasuredData *MeasuredData::create(const std::string &type,
                                   const std::string &trackerName,
                                   std::string path)
{
    if (type == "cout")
    {
        return new MeasuredDataStream(trackerName, std::cout);
    }
    else if (type == "file")
    {
        return new MeasuredDataFile(trackerName, path);
    }
    else
    {
        // unknown type
        throw std::runtime_error("Unknown data store type" + type);
    }
}