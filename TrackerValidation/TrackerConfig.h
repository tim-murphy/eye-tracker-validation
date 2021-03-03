// Eye tracker configuration. Although all eye trackers are different, many of
// them have similar configuration details (e.g. IP address).
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef TRACKERCONFIG_H
#define TRACKERCONFIG_H

#include <iosfwd>
#include <string>

class TrackerConfig
{
  public:
    std::string ipAddress;
    unsigned int ipPort;

    TrackerConfig(std::string ip, unsigned int port)
        : ipAddress(ip), ipPort(port)
    {}

    friend std::ostream &operator<<(std::ostream &str,
                                    const TrackerConfig &config);
};

#endif // defined TRACKERCONFIG_H
