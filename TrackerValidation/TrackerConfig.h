// Eye tracker configuration. Although all eye trackers are different, many of
// them have similar configuration details (e.g. IP address).

#ifndef TRACKERCONFIG_H
#define TRACKERCONFIG_H

#include <string>

struct TrackerConfig
{
    std::string ipAddress;
    unsigned int ipPort;
};

#endif // defined TRACKERCONFIG_H
