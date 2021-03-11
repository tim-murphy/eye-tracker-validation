// Tracker data collector for the SR Eyelink 1000 Plus system.
// Written by Tim Murphy <tim@murphy.org> 2021

#include "Eyelink1000PlusCollector.h"

#include "common.h"
#include "eyelink/core_expt.h"
#include "eyelink/eyelink.h"

#include <stdexcept>

void Eyelink1000PlusCollector::collectData()
{
    // connection mode
    // use the IP port to configure this
    static const int CONNECT_MODE = 0;
    static const int DUMMY_MODE = 1;

    const int mode = (config.ipPort == 0 ? CONNECT_MODE : DUMMY_MODE);

    // this function needs a non-const pointer :(
    char ipAddress[16]; memset(ipAddress, 0, sizeof(ipAddress));
    strncpy(ipAddress, config.ipAddress.c_str(), sizeof(ipAddress));
    ipAddress[sizeof(ipAddress) - 1] = '\0'; // ensure ending in null
    if (set_eyelink_address(ipAddress) != 0)
    {
        throw std::runtime_error("Invalid IP address for " + getName()
                                 + " :" + ipAddress);
    }

    if (open_eyelink_connection(mode) != 0)
    {
        throw std::runtime_error("Could not connect to " + getName());
    }

    if (mode == DUMMY_MODE)
    {
        eyelink_dummy_open();
    }
    else
    {
        eyelink_open();
    }

    // limit the amount of data recorded to improve performance
    eyecmd_printf("link_sample_data = LEFT,RIGHT");

    // set the screen resolution
    std::pair<unsigned int, unsigned int> res = common::getScreenRes();
    eyecmd_printf("screen_pixel_coords = 0 0 %i %i", res.first, res.second);

    // start sending samples through the link
    // note: params are 1=enable, 0=disable, in this order:
    //  - write samples to EDF file
    //  - write events to EDF file
    //  - send samples through link
    //  - send events through link
    if (start_recording(0, 0, 1, 0) != 0)
    {
        throw std::runtime_error("Could not start tracker " + getName());
    }

    if (!eyelink_wait_for_block_start(100, 1, 0))
    {
        throw std::runtime_error("No data available for tracker " + getName());
    }

    // FIXME change this to int if we can make it pixel coords
    ALL_DATA buf;
    memset(&buf, 0, sizeof(buf));

    if (DUMMY_MODE)
    {
        buf.is.flags = SAMPLE_LEFT + SAMPLE_RIGHT;
        buf.is.gx[0] = 880;
        buf.is.gx[1] = 882;
        buf.is.gy[0] = 421;
        buf.is.gy[1] = 420;
    }

    unsigned int xPos[2], yPos[2];
    while (isRunning())
    {
        if (eyelink_newest_sample(&buf))
        {
            std::pair<unsigned int, unsigned int> gazePos;

            // flags is a bitfield. Bitmask SAMPLE_LEFT for left data
            // available, SAMPLE_RIGHT for right data available.
            bool validData[2];
            for (int eye = LEFT_EYE; eye <= RIGHT_EYE; ++eye)
            {
                int bitmask = (eye == LEFT_EYE ? SAMPLE_LEFT : SAMPLE_RIGHT);
                validData[eye] = (buf.is.flags & bitmask) != 0;
            }

            for (int eye = LEFT_EYE; eye <= RIGHT_EYE; ++eye)
            {
                // FIXME do we need to divide these by eyelink_position_prescaler?
                xPos[eye] = buf.is.gx[eye];
                yPos[eye] = buf.is.gy[eye];

                if (xPos[eye] == MISSING_DATA || yPos[eye] == MISSING_DATA)
                {
                    validData[eye] = false;
                }
            }

            // if both eyes have data then use the average of the two
            if (validData[LEFT_EYE] && validData[RIGHT_EYE])
            {
                gazePos.first = (xPos[LEFT_EYE] + xPos[RIGHT_EYE]) / 2;
                gazePos.second = (yPos[LEFT_EYE] + yPos[RIGHT_EYE]) / 2;
            }
            else if (validData[LEFT_EYE])
            {
                gazePos.first = xPos[LEFT_EYE];
                gazePos.second = yPos[LEFT_EYE];
            }
            else if (validData[RIGHT_EYE])
            {
                gazePos.first = xPos[RIGHT_EYE];
                gazePos.second = yPos[RIGHT_EYE];
            }

            position.setCurrentPosition(gazePos, buf.is.time);
        }
    }

    stop_recording();
    close_eyelink_system();
}