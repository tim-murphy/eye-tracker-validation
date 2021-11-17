// Tracker data collector for the Gazepoint GP3 system.
// Written by Tim Murphy <tim@murphy.org> 2021

#include "GazepointGP3Collector.h"

#include "common.h"

#include "gazepoint/GPClient.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
    std::pair<unsigned int, unsigned int> calculatePos(
        bool valid,
        double x,
        double y,
        std::pair<unsigned int, unsigned int> screenRes)
    {
        std::pair<unsigned int, unsigned int> pos = std::make_pair(
            static_cast<unsigned int>(static_cast<double>(screenRes.first) *  x),
            static_cast<unsigned int>(static_cast<double>(screenRes.second) * y));

        // if the recording is invalid, don't use the values from the data packet
        if (!valid || x < 0.0 || x > 1.0)
        {
            pos.first = common::invalidCoord;
        }

        if (!valid || y < 0.0 || y > 1.0)
        {
            pos.second = common::invalidCoord;
        }

        return pos;
    }
}

void GazepointGP3Collector::collectData()
{
    // send the following to get set up:
    // <SET ID="ENABLE_SEND_COUNTER" STATE="1" />
    // <SET ID="ENABLE_SEND_POG_BEST" STATE="1" />
    // <SET ID="ENABLE_SEND_DATA" STATE="1" />

    // data will be received in the following format
    // <REC CNT="237217" BPOGX="0.43388" BPOGY="1.08536" BPOGV="1" />

    // note: if BPOGV == 1 means the measurement is valid, 0 means invalid
    //       and the BPOG[X|Y] values are the last known values.

    GPClient client(config.ipAddress, config.ipPort);
    client.client_connect();

    // screen config
    std::pair<unsigned int, unsigned int> screenRes = common::getScreenRes();
    std::stringstream screenConfig;
    screenConfig << "<SET ID=\"SCREEN_SIZE\" X=\"0\" Y=\"0\" WIDTH=\""
                 << screenRes.first << "\" HEIGHT=\""
                 << screenRes.second << "\" />";

    client.send_cmd(screenConfig.str());
    client.send_cmd("<SET ID=\"ENABLE_SEND_COUNTER\" STATE=\"1\" />");
    client.send_cmd("<SET ID=\"ENABLE_SEND_POG_RIGHT\" STATE=\"1\" />");
    client.send_cmd("<SET ID=\"ENABLE_SEND_POG_LEFT\" STATE=\"1\" />");
    client.send_cmd("<SET ID=\"ENABLE_SEND_DATA\" STATE=\"1\" />");

    while (isRunning())
    {
        try
        {
            std::string rxstr = client.get_rx_latest();

            // For efficiency sake, we assume the returned data is in the
            // correct format. This means we can avoid the overhead of an XML
            // parser and just pull out the data with regex :-)
            // <REC CNT="151747" LPOGX="0.87396" LPOGY="0.02765" LPOGV="1" RPOGX="0.89497" RPOGY="0.77830" RPOGV="1" />
            static const std::regex r(
                "^<REC CNT=\"(\\d+)\" LPOGX=\"([01]\\.\\d+)\" LPOGY=\"([01]\\.\\d+)\" LPOGV=\"([01])\" RPOGX=\"([01]\\.\\d+)\" RPOGY=\"([01]\\.\\d+)\" RPOGV=\"([01])\" />$");
            std::smatch matches;

            // regex_match throws a read access violation when the main
            // thread finishes but this is still running. This is caught below
            // and ignored - if we're exiting then there's nothing to recover
            if (std::regex_match(rxstr, matches, r))
            {
                // x,y values are a fraction of the screen size. These are
                // sometimes <0 or >1, which is not mentioned in the docs,
                // so we have ignored those values. The regex above will only
                // match values [0,2), so we will exclude (1,2) below.
                double id = std::stod(matches[1]);
                double xLeft = std::stod(matches[2]);
                double yLeft = std::stod(matches[3]);
                bool validLeft = (std::string(matches[4])[0] == '1' ? true : false);
                double xRight = std::stod(matches[5]);
                double yRight = std::stod(matches[6]);
                bool validRight = (std::string(matches[7])[0] == '1' ? true : false);

                std::pair<unsigned int, unsigned int> gazeRight
                    = ::calculatePos(validRight, xRight, yRight, screenRes);
                std::pair<unsigned int, unsigned int> gazeLeft
                    = ::calculatePos(validLeft, xLeft, yLeft, screenRes);

                position.setCurrentPositionRightLeft(gazeRight, gazeLeft, id);
            }
        }
        catch (std::exception &e)
        {
            // don't crash on exceptions, just try again
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
