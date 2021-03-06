﻿// Gaze tracker validation system
// Written by Tim Murphy <tim@murphy.org>

#include "Validator.h"

#include "common.h"
#include "TrackerConfig.h"
#include "ValidatorConfig.h"
#include "version.h"

#include <iostream>
#include <map>
#include <stdexcept>

#ifndef _WIN32
#include <getopt.h>
#endif

void printUsage(const char * const cmd)
{
    // for default values
    ValidatorConfig config;

    // Windows and linux/unix use different conventions for command line arguments.
    // We will use the convention for each system. It means the usage is slightly
    // different between platforms, but the software will seem more "native" and
    // user friendly.
#ifdef _WIN32
    const char flag = '/';
    const char equals = ':';
#else
    const char *flag = "--";
    const char equals = '=';
#endif

    std::cout << "Usage: " << cmd << " [options]" << std::endl
              << flag << "help\t\t\tdisplay this help text" << std::endl
              << flag << "preview\t\tdisplay all target locations on screen" << std::endl
              << flag << "cols" << equals << "<n>"
                    << "\t\tsplit screen into <n> columns (default "
                    << config.cols << ")" << std::endl
              << flag << "rows" << equals << "<n>"
                    << "\t\tsplit screen into <n> rows (default "
                    << config.rows << ")" << std::endl
              << flag << "repeats" << equals << "<n>"
                    << "\t\tnumber of times to test each point (default "
                    << config.repeats << ")" << std::endl
              << flag << "padding" << equals << "<n>"
                    << "\t\tmargin in pixels from screen edges which will be excluded in target location calculations" << std::endl
                    << "\t\t\t\t(default " << config.padding << ")" << std::endl
              << flag << "targtype" << equals << "<s>"
                    << "\t\tthe type of target (\"circle\" or \"crosshairbullseye\")" << std::endl
                    << "\t\t\t\t(default \"" << config.targType << "\")" << std::endl
              << flag << "targsize" << equals << "<n>"
                    << "\t\tdiameter of target in pixels (default "
                    << config.targetSize << ")" << std::endl
              << flag << "targlocation" << equals << "<s>"
                    << "\twhere in the grid cell to place the targets (\"middle\" or \"corners\")" << std::endl
                    << "\t\t\t\t(default \"" << config.targLocation << "\")" << std::endl
              << flag << "label" << equals << "<s>"
                    << "\t\tlabel for this experiment (default \""
                    << config.trackerLabel << "\")" << std::endl
              << flag << "outputfile" << equals << "<s>"
                    << "\t\tpath to file to write output data to, or leave empty to write to console" << std::endl
                    << "\t\t\t\t(default \"" << config.outputFile << "\")" << std::endl
              << flag << "tracker" << equals << "<s>"
                    << "\t\tthe tracker being tested (\"mouse\", \"eyelink\" or \"GP3\")" << std::endl
                    << "\t\t\t\t(default \"" << config.tracker << "\")" << std::endl
              << flag << "trackerip" << equals << "<s>"
                    << "\t\tIP address used by the tracker (tracker default if not set)" << std::endl
              << flag << "trackerport" << equals << "<n>"
                    << "\tIP port used by the tracker (tracker default if not set)" << std::endl
              << flag << "subject" << equals << "<s>"
                    << "\t\tname or ID of the subject under test, or \"\" for a prompt (default \""
                    << config.subject << "\")" << std::endl;
}

int main(int argc, char *argv[])
{
    std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl
              << "Gaze tracker validation tool" << std::endl
              << "Version: " << softwareVersion() << std::endl
              << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;

    // default config
    ValidatorConfig config;

    // store command line args in here (different logic for different systems)
    // to allow for a single parsing routine
    std::map<std::string, std::string> cmdArgs;
    bool configSuccess = true;

#ifndef _WIN32
    // grab the config from the command line
    static const char * const cmdShort = "c:hl:r:n:t:g:c:s:i:p:o:u:";
    static const struct option cmdOpts[] = {
        {"help",        no_argument,       nullptr, 'h'},
        {"preview",     no_argument,       nullptr, 'x'},
        {"cols",        required_argument, nullptr, 'c'},
        {"rows",        required_argument, nullptr, 'r'},
        {"repeats",     required_argument, nullptr, 'n'},
        {"padding",     required_argument, nullptr, 'm'},
        {"targsize",    required_argument, nullptr, 't'},
        {"targtype",    required_argument, nullptr, 'g'},
        {"targlocation",required_argument, nullptr, 'z'},
        {"label",       required_argument, nullptr, 'l'},
        {"tracker",     required_argument, nullptr, 's'},
        {"trackerip",   required_argument, nullptr, 'i'},
        {"trackerport", required_argument, nullptr, 'p'},
        {"outputfile",  required_argument, nullptr, 'o'},
        {"subject",     required_argument, nullptr, 'u'},
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

    // if we don't configure any IP settings, use the defaults
    bool trackerIPAddressConfigured = false;
    bool trackerIPPortConfigured = false;

    // parse the command line arguments
    // using C++11 syntax for wider compatibility
    // FIXME add bounds checking
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
        else if (key == "padding")
        {
            int intval = std::atoi(val.c_str());
            if (intval < 0)
            {
                std::cerr << "ERROR: padding value must be positive"
                          << std::endl;
                configSuccess = false;
            }
            else
            {
                config.padding = static_cast<unsigned int>(intval);
            }
        }
        else if (key == "targsize")
        {
            config.targetSize = std::atoi(val.c_str());
        }
        else if (key == "targtype")
        {
            config.targType = val;
        }
        else if (key == "targlocation")
        {
            if (val == "middle" || val == "corners")
            {
                config.targLocation = val;
            }
            else
            {
                std::cerr << "ERROR: targlocation must be either \"middle\" "
                          << "or \"corners\"" << std::endl;
                configSuccess = false;
            }
        }
        else if (key == "label")
        {
            config.trackerLabel = val;
        }
        else if (key == "outputfile")
        {
            config.outputFile = val;
        }
        else if (key == "system" || key == "tracker")
        {
            config.tracker = val;
        }
        else if (key == "subject")
        {
            config.subject = val;

            // if not set, get from a prompt
            while (config.subject == "")
            {
                std::cout << "Please enter a subject name or ID: " << std::flush;
                std::getline(std::cin >> std::ws, config.subject);
            }
        }
        else if (key == "trackerip")
        {
            config.trackerConfig.ipAddress = val;
            trackerIPAddressConfigured = true;
        }
        else if (key == "trackerport")
        {
            config.trackerConfig.ipPort = std::atoi(val.c_str());
            trackerIPPortConfigured = true;
        }
        else if (key == "help")
        {
            // this will trigger the help message to be shown
            configSuccess = false;
        }
        else if (key == "preview")
        {
            config.preview = true;
        }
        else
        {
            std::cerr << "Error: Invalid argument: " << key << std::endl;
            configSuccess = false;
        }
    }

    // load tracker default IP config
    // FIXME move this to the tracker class
    std::map<std::string, std::pair<std::string, unsigned int> > defaultIP;
    defaultIP["gp3"] = std::make_pair("127.0.0.1", 4242);
    defaultIP["GP3"] = defaultIP["gp3"];
    defaultIP["eyelink"] = std::make_pair("100.1.1.1", 0);

    if (defaultIP.find(config.tracker) != defaultIP.end())
    {
        if (!trackerIPAddressConfigured)
        {
            config.trackerConfig.ipAddress = defaultIP[config.tracker].first;
        }
        if (!trackerIPPortConfigured)
        {
            config.trackerConfig.ipPort = defaultIP[config.tracker].second;
        }
    }

    if (!configSuccess)
    {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::pair<unsigned int, unsigned int> screenRes = common::getScreenRes();
    std::cout << "Screen resolution: " << screenRes.first << "x"
              << screenRes.second << std::endl;

    std::cout << config << std::endl;

    try
    {
        Validator v = Validator(config);
        v.startTrackerDataCollector();
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
