// Gaze tracker validation system
// Written by Tim Murphy <tim@murphy.org>

#include "Validator.h"

#include "TrackerConfig.h"

#include <iostream>
#include <map>
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
              << "--targtype <s> the type of target (\"circle\" or \"crosshairbullseye\")" << std::endl
              << "                 (default \"crosshairbullseye\")" << std::endl
              << "--targsize <n> diameter of target in pixels (default 5)" << std::endl
              << "--label <s>    label for this experiment (default \"dummy\")" << std::endl
              << "--system <s>   the system under test (\"mouse\" (default) or \"GP3\")" << std::endl;
}

#else // if _WIN32 is defined

void printUsage(const char * const cmd)
{
    std::cout << "Usage: " << cmd << " [options]" << std::endl
              << "/help         display this help text" << std::endl
              << "/cols:<n>     split screen into <n> columns (default 3)" << std::endl
              << "/rows:<n>     split screen into <n> rows (default 3)" << std::endl
              << "/repeats:<n>  number of times to test each point (default 5)" << std::endl
              << "/targtype:<s> the type of target (\"circle\" or \"crosshairbullseye\")" << std::endl
              << "                (default \"crosshairbullseye\")" << std::endl
              << "/targsize:<n> diameter of target in pixels (default 5)" << std::endl
              << "/label:<s>    label for this experiment (default \"dummy\")" << std::endl
              << "/system:<s>   the system under test (\"mouse\" (default) or \"GP3\")" << std::endl;
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

    // store command line args in here (different logic for different systems)
    // to allow for a single parsing routine
    std::map<std::string, std::string> cmdArgs;
    bool configSuccess = true;

#ifndef _WIN32
    // grab the config from the command line
    static const char * const cmdShort = "c:hl:r:n:t:g:c:s:";
    static const struct option cmdOpts[] = {
        {"help",     no_argument,       nullptr, 'h'},
        {"cols",     required_argument, nullptr, 'c'},
        {"rows",     required_argument, nullptr, 'r'},
        {"repeats",  required_argument, nullptr, 'n'},
        {"targsize", required_argument, nullptr, 't'},
        {"targtype", required_argument, nullptr, 'g'},
        {"label",    required_argument, nullptr, 'l'},
        {"system",   required_argument, nullptr, 's'},
        {nullptr,    no_argument,       nullptr, 0}
    };

    int longIndex = 0;
    char opt;
    while ((opt = getopt_long(argc, argv, cmdShort, cmdOpts, &longIndex)) != -1)
    {
        switch (opt)
        {
          case '?': // invalid argument
            // this prints an error message by default, no need to do it again
            configSuccess = false;
            break;
          case ':': // missing value
            std::cerr << "Error: Argument requires a value: " << cmdOpts[longIndex].name << std::endl;
            configSuccess = false;
            break;
          default:
            std::string val = "";
            if (optarg != nullptr)
            {
                val = optarg;
            }
            cmdArgs[cmdOpts[longIndex].name] = val;
        }
    }
#else // if _WIN32 is defined
    for (int i = 1; i < argc; ++i)
    {
        // must start with '/' or it's not a valid argument
        if (argv[i][0] != '/')
        {
            std::cerr << "Error: Invalid argument: " << argv[i] << std::endl;
            configSuccess = false;
            continue;
        }

        // strip the '/' from the argument
        const std::string argString(argv[i]+1);

        // windows arguments are delimited with a comma
        static const char delimeter = ':';
        std::string key("");
        std::string val("");

        auto pos = argString.find(delimeter, 0);
        if (pos == std::string::npos)
        {
            // no argument given
            key = argString;
        }
        else
        {
            key = argString.substr(0, pos);
            val = argString.substr(pos+1);
        }

        cmdArgs[key] = val;
    }
#endif // not defined _WIN32

    // parse the command line arguments
    // using C++11 syntax for wider compatibility
    for (const auto &kvpair : cmdArgs)
    {
        const std::string &key = kvpair.first;
        const std::string &val = kvpair.second;
        if (key == "cols")
        {
            cols = std::atoi(val.c_str());
        }
        else if (key == "rows")
        {
            rows = std::atoi(val.c_str());
        }
        else if (key == "repeats")
        {
            repeats = std::atoi(val.c_str());
        }
        else if (key == "targsize")
        {
            targetSize = std::atoi(val.c_str());
        }
        else if (key == "targtype")
        {
            targType = val;
        }
        else if (key == "label")
        {
            trackerLabel =val;
        }
        else if (key == "system")
        {
            system = val;
        }
        else if (key == "help")
        {
            // this will trigger the help message to be shown
            configSuccess = false;
        }
        else
        {
            std::cerr << "Error: Invalid argument: " << key << std::endl;
            configSuccess = false;
        }
    }

    if (!configSuccess)
    {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::cout << "Using config:" << std::endl
              << "  cols=" << cols << std::endl
              << "  rows=" << rows << std::endl
              << "  repeats=" << repeats << std::endl
              << "  targsize=" << targetSize << std::endl
              << "  targtype=" << targType << std::endl
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
    std::cout << "Finished! Press ENTER to exit" << std::endl;
    std::cin.get();
    std::cout << "Have a nice day :)" << std::endl;

    return EXIT_SUCCESS;
}
