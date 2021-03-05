// OpenGL helper functions

#include "OpenGLCommon.h"

#include "common.h"

std::pair<double, double> OpenGLPixelToPosition(
    unsigned int xPixel, unsigned int yPixel)
{
    // Can be done by 2 x (actual_pixel_value / max_pixel_value) - 1
    // for each axis.
    // e.g. location (100, 100) on a 1024x768 screen would be:
    //  x = 2 x (100/1024) - 1 = -0.80
    //  y = 1 - (2 x (100/768) = 0.74
    //  So returned value would be (-0.80, 0.74)
    std::pair<unsigned int, unsigned int> res = common::getScreenRes();
    double xPos = 2 * (static_cast<double>(xPixel)
                  / static_cast<double>(res.first)) - 1.0;
    double yPos = 1.0 - (2 * (static_cast<double>(yPixel)
                  / static_cast<double>(res.second)));

    return std::make_pair(xPos, yPos);
}
