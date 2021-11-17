// Tracker data collector for the SR Eyelink 1000 Plus system.
// Written by Tim Murphy <tim@murphy.org> 2021

#include "Eyelink1000PlusCollector.h"

#include "common.h"
#include "eyelink/core_expt.h"
#include "eyelink/eyelink.h"

#include <cstring>
#include <stdexcept>

void Eyelink1000PlusCollector::collectData()
{
    // connection mode
    // use the IP port to configure this
    static const int CONNECT_MODE = 0;
    static const int DUMMY_MODE = 1;

    const int mode = (config.ipPort == 0 ? CONNECT_MODE : DUMMY_MODE);

    // this function needs a non-const pointer :(
    char ipAddress[16];
    std::memset(ipAddress, 0, sizeof(ipAddress));
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

    // set the screen resolution
    std::pair<unsigned int, unsigned int> res = common::getScreenRes();
    eyecmd_printf("screen_pixel_coords = 0 0 %i %i", res.first, res.second);

    // eyecmd_printf can take up to 500ms to process
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // start sending samples through the link
    // note: params are 1=enable, 0=disable, in this order:
    //  - write samples to EDF file
    //  - write events to EDF file
    //  - send samples through link
    //  - send events through link
    int err = start_recording(0, 0, 1, 0);
    if (err != 0)
    {
        throw std::runtime_error("Could not connect to tracker " + getName()
                                 + " at " + config.ipAddress + " (error "
                                 + std::to_string(err).c_str() + ")");
    }

    if (!eyelink_wait_for_block_start(100, 1, 0))
    {
        throw std::runtime_error("No data available for tracker " + getName());
    }

    ALL_DATA buf;
    std::memset(&buf, 0, sizeof(buf));

    if (DUMMY_MODE)
    {
        // if we're faking a connection, fake some valid data while we're at it
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
            // flags is a bitfield. Bitmask SAMPLE_LEFT for left data
            // available, SAMPLE_RIGHT for right data available.
            bool validData[2];
            for (int eye = LEFT_EYE; eye <= RIGHT_EYE; ++eye)
            {
                int bitmask = (eye == LEFT_EYE ? SAMPLE_LEFT : SAMPLE_RIGHT);
                validData[eye] = (buf.is.flags & bitmask) != 0;
            
                xPos[eye] = buf.is.gx[eye] / eyelink_position_prescaler();
                yPos[eye] = buf.is.gy[eye] / eyelink_position_prescaler();

                if (!validData[eye] || xPos[eye] == MISSING_DATA)
                {
                    xPos[eye] = common::invalidCoord;
                }

                if (!validData[eye] || yPos[eye] == MISSING_DATA)
                {
                    yPos[eye] = common::invalidCoord;
                }
            }

            std::pair<unsigned int, unsigned int> gazePosRight
                = std::make_pair(xPos[RIGHT_EYE], yPos[RIGHT_EYE]);
            std::pair<unsigned int, unsigned int> gazePosLeft
                = std::make_pair(xPos[LEFT_EYE], yPos[LEFT_EYE]);

            position.setCurrentPositionRightLeft(gazePosRight, gazePosLeft, buf.is.time);
        }
    }

    stop_recording();
    close_eyelink_system();
}
