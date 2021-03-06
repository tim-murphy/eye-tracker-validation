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
    unsigned int padding;
    unsigned int targetSize;
    std::string targType;
    std::string targLocation;
    std::string outputFile;
    std::string trackerLabel;
    std::string tracker;
    std::string subject;
    TrackerConfig trackerConfig;
    bool preview;

    ValidatorConfig(unsigned int columns = 5,
                    unsigned int rows = 3,
                    unsigned int repeats = 2,
                    unsigned int padding = 0,
                    unsigned int targetSize = 6,
                    std::string targType = "crosshairbullseye",
                    std::string targLocation = "middle",
                    std::string trackerLabel = "Test project",
                    std::string tracker = "mouse",
                    std::string subject = "Test user",
                    std::string outputFile = "",
                    bool preview = false,
                    TrackerConfig trackerConfig = TrackerConfig("127.0.0.1", 4242))
        : cols(columns), rows(rows), repeats(repeats), padding(padding),
          targetSize(targetSize), targType(targType), targLocation(targLocation),
          trackerLabel(trackerLabel), tracker(tracker),
          trackerConfig(trackerConfig), subject(subject),
          preview(preview), outputFile(outputFile)
    {}

    friend std::ostream& operator<<(std::ostream & str,
                                    const ValidatorConfig & config);
};

#endif // defined VALIDATORCONFIG_H