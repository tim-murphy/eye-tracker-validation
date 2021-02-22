// Gaze tracker validation system
// Written by Tim Murphy <tim@murphy.org>

#include "Validator.h"

#include "TrackerConfig.h"

#include <iostream>
#include <stdexcept>

#ifndef _WIN32
#include <getopt.h>

// TODO get default values dynamically
void printUsage(const char * const cmd)
{
    std::cout << "Usage: " << cmd << " [options]" << std::endl
              << "--help         display this help text" << std::endl
              << "--cols <n>     split screen into <n> columns (default 3)" << std::endl
              << "--rows <n>     split screen into <n> rows (default 3)" << std::endl
              << "--repeats <n>  number of times to test each point (default 5)" << std::endl
              << "--targsize <n> diameter of target in pixels (default 5)" << std::endl
              << "--label <s>    label for this experiment (default \"dummy\")" << std::endl
              << "--system <s>   the system under test (\"mouse\" (default) or \"GP3\")" << std::endl;
}
#endif // not defined _WIN32

int main(int argc, char *argv[])
{
    std::cout << "Gaze tracker validation tool" << std::endl
              << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;

    // default config
    unsigned int cols = 5;
    unsigned int rows = 3;
    unsigned int repeats = 5;
    unsigned int targetSize = 6;
    std::string targType = "crosshairbullseye";
    std::string trackerLabel = "dummy";
    std::string system = "mouse";

#ifndef _WIN32
    // grab the config from the command line
    static const char * const cmdShort = "c:hl:r:n:c:s:";
    static const struct option cmdOpts[] = {
        {"help",     no_argument,       nullptr, 'h'},
        {"cols",     required_argument, nullptr, 'c'},
        {"rows",     required_argument, nullptr, 'r'},
        {"repeats",  required_argument, nullptr, 'n'},
        {"targsize", required_argument, nullptr, 't'},
        {"label",    required_argument, nullptr, 'l'},
        {"system",   required_argument, nullptr, 's'},
        {nullptr,    no_argument,       nullptr, 0}
    };

    bool configSuccess = true; // will be set to false if any errors below
    int longIndex = 0;
    char opt;
    while ((opt = getopt_long(argc, argv, cmdShort, cmdOpts, &longIndex)) != -1)
    {
        switch (opt)
        {
        case 'c':
            cols = std::atoi(optarg);
            break;
        case 'r':
            rows = std::atoi(optarg);
            break;
        case 'n':
            repeats = std::atoi(optarg);
            break;
        case 't':
            targetSize = std::atoi(optarg);
            break;
        case 'l':
            trackerLabel = std::string(optarg);
            break;
        case 's':
            system = std::string(optarg);
            break;
        case '?': // invalid argument
            std::cerr << "Error: Invalid argument: " << static_cast<char>(optopt) << std::endl;
            configSuccess = false;
            break;
        case ':': // missing value
            std::cerr << "Error: Argument requires a value: " << cmdOpts[longIndex].name << std::endl;
            configSuccess = false;
            break;
        case 'h':
            configSuccess = false;
            break;
        }
    }

    if (!configSuccess)
    {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }
#endif // not defined _WIN32

    std::cout << "Using config:" << std::endl
              << "  cols=" << cols << std::endl
              << "  rows=" << rows << std::endl
              << "  repeats=" << repeats << std::endl
              << "  targsize=" << targetSize << std::endl
              << "  targType=" << targType << std::endl
              << "  label=" << trackerLabel << std::endl
              << "  system=" << system << std::endl;

    // standard configuration for Gazepoint
    TrackerConfig gpConfig;
    gpConfig.ipAddress = "127.0.0.1";
    gpConfig.ipPort = 4242;

    try
    {
        Validator v = Validator(cols, rows, repeats,
                                targType, targetSize, trackerLabel);
        v.startTrackerDataCollector(system, gpConfig);
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
