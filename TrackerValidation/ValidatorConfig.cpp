// config for the main validation program
// Written by Tim Murphy <tim@murphy.org> 2021

#include "ValidatorConfig.h"

#include <iostream>

std::ostream &operator<<(std::ostream &str,
                         const ValidatorConfig &config)
{
    str << "Validator configuration:" << std::endl
        << "  cols = " << config.cols << std::endl
        << "  rows = " << config.rows << std::endl
        << "  repeats = " << config.repeats << std::endl
        << "  targsize = " << config.targetSize << std::endl
        << "  targtype = " << config.targType << std::endl
        << "  trackerlabel = " << config.trackerLabel << std::endl
        << "  tracker = " << config.tracker << std::endl
        << "  trackerConfig = " << config.trackerConfig << std::endl
        << "  subject = " << config.subject << std::endl
        << "  outputFile = " << config.outputFile << std::endl;
    return str;
}