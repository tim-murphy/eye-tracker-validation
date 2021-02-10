// Gaze tracker validation system
// Written by Tim Murphy <tim@murphy.org>

#include "Validator.h"

#include "TrackerConfig.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[])
{
    std::cout << "Gaze tracker validation tool" << std::endl;

    // TODO get this from config or command line args (or keyboard input?)
    // TODO also put this in a config struct
    unsigned int cols = 5;
    unsigned int rows = 3;
    unsigned int repeats = 5;
    unsigned int targetSize = 5;
    std::string trackerLabel = "Dummy tracker #1";

    // standard configuration for Gazepoint
    TrackerConfig gpConfig {
        .ipAddress="127.0.0.1",
        .ipPort=4242
    };

    try
    {
        Validator v = Validator(cols, rows, repeats, targetSize, trackerLabel);
        v.startTrackerDataCollector("dummy", gpConfig);
        v.startUI(&argc, argv); // this will stop when it has finished

        // processing happens here

        v.stopTrackerDataCollector();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Finished! Have a nice day :)" << std::endl;

    return EXIT_SUCCESS;
}
