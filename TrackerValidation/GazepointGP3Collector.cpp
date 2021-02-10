// Tracker data collector for the Gazepoint GP3 system.
// Written by Tim Murphy <tim@murphy.org> 2021

#include "GazepointGP3Collector.h"

#include "ValidatorUI.h" // for screen resolution

#include "gazepoint/GPClient.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

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
    std::pair<unsigned int, unsigned int> screenRes = ValidatorUI::getScreenRes();
    std::stringstream screenConfig;
    screenConfig << "<SET ID=\"SCREEN_SIZE\" X=\"0\" Y=\"0\" WIDTH=\""
                 << screenRes.first << "\" HEIGHT=\""
                 << screenRes.second << "\" />";

    client.send_cmd(screenConfig.str());
    client.send_cmd("<SET ID=\"ENABLE_SEND_COUNTER\" STATE=\"1\" />");
    client.send_cmd("<SET ID=\"ENABLE_SEND_POG_BEST\" STATE=\"1\" />");
    client.send_cmd("<SET ID=\"ENABLE_SEND_DATA\" STATE=\"1\" />");

    while (isRunning())
    {
        try
        {
            std::string rxstr = client.get_rx_latest();

            // For efficiency sake, we assume the returned data is in the
            // correct format. This means we can avoid the overhead of an XML
            // parser and just pull out the data with regex :-)
            static const std::regex r(
                "^<REC CNT=\"(\\d+)\" BPOGX=\"([01]\\.\\d+)\" BPOGY=\"([01]\\.\\d+)\" BPOGV=\"1\" />$");
            std::smatch matches;

            // FIXME regex_match throws a read access violation when the main
            // thread finishes but this is still running
            if (std::regex_match(rxstr, matches, r))
            {
                // x,y values are a fraction of the screen size. These are
                // sometimes <0 or >1, which is not mentioned in the docs,
                // so we have ignored those values. The regex above will only
                // match values [0,2), so we will exclude (1,2) below.
                double id = std::stod(matches[1]);
                double x = std::stod(matches[2]);
                double y = std::stod(matches[3]);

                // exclude invalid values
                if (x > 1.0 || y > 1.0)
                {
                    continue;
                }

                // convert the x,y fractions into pixels
                unsigned int xPix = static_cast<unsigned int>(
                    static_cast<double>(screenRes.first) *  x);
                unsigned int yPix = static_cast<unsigned int>(
                    static_cast<double>(screenRes.second) * y);
                position.setCurrentPosition(std::make_pair(xPix, yPix), id);
            }
        }
        catch (std::exception &e)
        {
            // don't crash on exceptions, just try again
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
