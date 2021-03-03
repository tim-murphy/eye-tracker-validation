// Eye tracker configuration. Although all eye trackers are different, many of
// them have similar configuration details (e.g. IP address).
// Written by Tim Murphy <tim@murphy.org> 2021

#include "TrackerConfig.h"

#include <iostream>

std::ostream &operator<<(std::ostream &str,
                         const TrackerConfig &config)
{
    str << "{ ipAddress=" << config.ipAddress
        << ", port=" << config.ipPort << " }";
    return str;
}