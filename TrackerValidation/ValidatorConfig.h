// config for the main validation program
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef VALIDATORCONFIG_H
#define VALIDATORCONFIG_H

#include "TrackerConfig.h"

#include <iosfwd>
#include <string>

class ValidatorConfig
{
  public:
    unsigned int cols;
    unsigned int rows;
    unsigned int repeats;
    unsigned int targetSize;
    std::string targType;
    std::string outputFile;
    std::string trackerLabel;
    std::string tracker;
    std::string subject;
    TrackerConfig trackerConfig;

    ValidatorConfig(unsigned int columns = 5,
                    unsigned int rows = 3,
                    unsigned int repeats = 5,
                    unsigned int targetSize = 6,
                    std::string targType = "crosshairbullseye",
                    std::string trackerLabel = "dummy",
                    std::string tracker = "mouse",
                    std::string subject = "test_user",
                    std::string outputFile = "",
                    TrackerConfig trackerConfig = TrackerConfig("127.0.0.1", 4242))
        : cols(columns), rows(rows), repeats(repeats),
          targetSize(targetSize), targType(targType),
          trackerLabel(trackerLabel), tracker(tracker),
          trackerConfig(trackerConfig), subject(subject)
    {}

    friend std::ostream& operator<<(std::ostream & str,
                                    const ValidatorConfig & config);
};

#endif // defined VALIDATORCONFIG_H