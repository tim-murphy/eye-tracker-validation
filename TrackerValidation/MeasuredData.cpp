// Abstract class for storage of measured data.
// Tim Murphy <tim@murphy.org> 2021

#include "MeasuredData.h"

#include "MeasuredDataFile.h"
#include "MeasuredDataStream.h"

#include <iostream>
#include <stdexcept>

MeasuredData::MeasuredData(const std::string &label,
                           const std::string &trackerName)
    : label(label), trackerName(trackerName)
{}

MeasuredData::~MeasuredData()
{
    // ensure any buffered data is written
    writeBuffer();
}

const std::string &MeasuredData::getLabel() const
{
    return label;
}

const std::string &MeasuredData::getTrackerName() const
{
    return trackerName;
}

void MeasuredData::writeBuffer()
{ }

MeasuredData *MeasuredData::create(const std::string &type,
                                   const std::string &label,
                                   const std::string &trackerName,
                                   std::string path)
{
    if (type == "cout")
    {
        return new MeasuredDataStream(label, trackerName, std::cout);
    }
    else if (type == "file")
    {
        return new MeasuredDataFile(label, trackerName, path);
    }
    else
    {
        // unknown type
        throw std::runtime_error("Unknown data store type" + type);
    }
}