// Gaze tracker validation system
// Written by Tim Murphy <tim@murphy.org>

#include "Validator.h"

#include "TrackerConfig.h"
#include "ValidatorConfig.h"

#include <iostream>
#include <map>
#include <stdexcept>

#ifndef _WIN32
#include <getopt.h>
#endif

void printUsage(const char * const cmd, const ValidatorConfig &config)
{
// Windows and linux/unix use different conventions for command line arguments.
// We will use the convention for each system. It means the usage is slightly
// different between platforms, but the software will seem more "native" and
// user friendly.
#ifdef _WIN32
    const char flag = '/';
    const char equals = ':';
#else
    const char[] flag = "--";
    const char equals = '=';
#endif

    std::cout << "Usage: " << cmd << " [options]" << std::endl
              << flag << "help         display this help text" << std::endl
              << flag << "cols" << equals << "<n>"
                    << "     split screen into <n> columns (default "
                    << config.cols << ")" << std::endl
              << flag << "rows" << equals << "<n>"
                    << "     split screen into <n> rows (default "
                    << config.rows << ")" << std::endl
              << flag << "repeats" << equals << "<n>"
                    << "  number of times to test each point (default "
                    << config.repeats << ")" << std::endl
              << flag << "targtype" << equals << "<s>"
                    << " the type of target (\"circle\" or \"crosshairbullseye\")" << std::endl
                    << "                (default \"" << config.targType << "\")" << std::endl
              << flag << "targsize" << equals << "<n>"
                    << " diameter of target in pixels (default "
                    << config.targetSize << ")" << std::endl
              << flag << "label" << equals << "<s>"
                    << "    label for this experiment (default \""
                    << config.trackerLabel << "\")" << std::endl
              << flag << "tracker" << equals << "<s>"
                    << "  the tracker being tested (\"mouse\" or \"GP3\")" << std::endl
                    << "                (default \"" << config.tracker << "\")" << std::endl;
}

int main(int argc, char *argv[])
{
    std::cout << "Gaze tracker validation tool" << std::endl
              << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;

    // default config
    ValidatorConfig config;

    // store command line args in here (different logic for different systems)
    // to allow for a single parsing routine
    std::map<std::string, std::string> cmdArgs;
    bool configSuccess = true;

#ifndef _WIN32
    // grab the config from the command line
    static const char * const cmdShort = "c:hl:r:n:t:g:c:s:";
    static const struct option cmdOpts[] = {
        {"help",        no_argument,       nullptr, 'h'},
        {"cols",        required_argument, nullptr, 'c'},
        {"rows",        required_argument, nullptr, 'r'},
        {"repeats",     required_argument, nullptr, 'n'},
        {"targsize",    required_argument, nullptr, 't'},
        {"targtype",    required_argument, nullptr, 'g'},
        {"label",       required_argument, nullptr, 'l'},
        {"tracker",     required_argument, nullptr, 's'},
        {"trackerip",   required_argument, nullptr, 'i'},
        {"trackerport", required_argument, nullptr, 'p'},
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
            config.cols = std::atoi(val.c_str());
        }
        else if (key == "rows")
        {
            config.rows = std::atoi(val.c_str());
        }
        else if (key == "repeats")
        {
            config.repeats = std::atoi(val.c_str());
        }
        else if (key == "targsize")
        {
            config.targetSize = std::atoi(val.c_str());
        }
        else if (key == "targtype")
        {
            config.targType = val;
        }
        else if (key == "label")
        {
            config.trackerLabel =val;
        }
        else if (key == "system" || key == "tracker")
        {
            config.tracker = val;
        }
        else if (key == "trackerip")
        {
            config.trackerConfig.ipAddress = val;
        }
        else if (key == "trackerport")
        {
            config.trackerConfig.ipPort = std::atoi(val.c_str());
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
        printUsage(argv[0], config);
        return EXIT_FAILURE;
    }

    std::cout << config << std::endl;

    try
    {
        Validator v = Validator(config.cols, config.rows, config.repeats,
                                config.targType, config.targetSize, config.trackerLabel);
        v.startTrackerDataCollector(config.tracker, config.trackerConfig);
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
