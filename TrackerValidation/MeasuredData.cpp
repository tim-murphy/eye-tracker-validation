// Abstract class for storage of measured data.
// Tim Murphy <tim@murphy.org> 2021

#include "MeasuredData.h"

#include "MeasuredDataFile.h"
#include "MeasuredDataStream.h"

#include <iostream>
#include <stdexcept>

MeasuredData::MeasuredData(const std::string &label,
                           const std::string &trackerName,
                           const std::string &subject)
    : label(label), trackerName(trackerName), subject(subject)
{}

MeasuredData::~MeasuredData()
{
    // ensure any buffered data is written
    writeBuffer();
}

const std::string &MeasuredData::getLabel(void) const
{
    return label;
}

const std::string &MeasuredData::getTrackerName(void) const
{
    return trackerName;
}

const std::string &MeasuredData::getSubject(void) const
{
    return subject;
}

void MeasuredData::writeBuffer()
{ }

MeasuredData *MeasuredData::create(const std::string &type,
                                   const std::string &label,
                                   const std::string &trackerName,
                                   const std::string &subject,
                                   std::string path)
{
    if (type == "cout")
    {
        return new MeasuredDataStream(label, trackerName, subject, std::cout);
    }
    else if (type == "file")
    {
        return new MeasuredDataFile(label, trackerName, subject, path);
    }
    else
    {
        // unknown type
        throw std::runtime_error("Unknown data store type" + type);
    }
}