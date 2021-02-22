// Fixation target abstract class
// Written by Tim Murphy <tim@murphy.org> 2021

#include "FixationTarget.h"

#include "CircleTarget.h"

#include <stdexcept>

FixationTarget::FixationTarget(unsigned int diameter)
    :diameter(diameter)
{ }

unsigned int FixationTarget::getDiameter(void) const
{
    return diameter;
}

FixationTarget *FixationTarget::create(const std::string &type,
                                       unsigned int diameter)
{
    if (type == "circle")
    {
        return new CircleTarget(diameter);
    }
    else
    {
        std::string err("Unknown fixation target type: " + type);
        throw std::runtime_error(err.c_str());
    }
}