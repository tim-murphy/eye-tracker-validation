// Gaze tracker validation system
// Written by Tim Murphy <tim@murphy.org>
// TODO: licence conditions here (LGPL?)

#include "Validator.h"

#include <iostream>

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

    Validator v = Validator(cols, rows, repeats, targetSize, trackerLabel);
    v.startTrackerDataCollector("GP3");
    v.startUI(&argc, argv); // this will stop when it has finished

    // processing happens here

    v.stopTrackerDataCollector();
    std::cout << "Finished! Have a nice day :)" << std::endl;
}
