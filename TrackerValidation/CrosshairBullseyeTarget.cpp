// Bullseye target (large circle with small inner circle) with crosshairs
// Noted as the most stable fixation target compared to others:
// https://www.sciencedirect.com/science/article/pii/S0042698912003380
// Written by Tim Murphy <tim@murphy.org> 2021

#include "CrosshairBullseyeTarget.h"

#include "common.h" // for common::pi
#include "OpenGLCommon.h"

#include <cmath>
#include <GL/freeglut.h>
#include <utility> // for std::pair

CrosshairBullseyeTarget::CrosshairBullseyeTarget(unsigned int diameter)
    : FixationTarget(diameter)
{}

unsigned int CrosshairBullseyeTarget::getOuterDiameter(void) const
{
    return getDiameter() * targetRatio;
}

void CrosshairBullseyeTarget::drawCircleOpenGL(unsigned int x, unsigned int y,
                                         unsigned int diameter)
{
    glBegin(GL_TRIANGLE_FAN);

    double radius = static_cast<double>(diameter) / 2.0;
    std::pair<double, double> centerPos = OpenGLPixelToPosition(x, y);
    glVertex2d(centerPos.first, centerPos.second);

    for (double n = 0; n <= segments; ++n)
    {
        // the angle used for this triangle segment
        double sigma = n * common::pi * 2.0 / static_cast<double>(segments);

        std::pair<double, double> pos = OpenGLPixelToPosition(
            static_cast<unsigned int>(x + (radius * cos(sigma))),
            static_cast<unsigned int>(y + (radius * sin(sigma))));

        glVertex2d(pos.first, pos.second);
    }

    glEnd();
}

void CrosshairBullseyeTarget::drawOpenGL(unsigned int x, unsigned int y)
{
    // first draw the outer circle (white)
    glColor4d(1.0, 1.0, 1.0, 1.0);
    drawCircleOpenGL(x, y, getOuterDiameter());

    // crosshair (black)
    // each rectangle covers (origin +/- outerRadius) x (origin +/- radius)
    // we extend the lines a bit to make sure they fully cover the circle
    glColor4d(0.0, 0.0, 0.0, 1.0);
    unsigned int radius = getDiameter() / 2;
    unsigned int outerRadius = getOuterDiameter() / 2;
    std::pair<double, double> upperLeftPos[] = {
        OpenGLPixelToPosition(x - outerRadius - 1, y - radius),
        OpenGLPixelToPosition(x - radius, y - outerRadius - 1)
    };
    std::pair<double, double> lowerRightPos[] = {
        OpenGLPixelToPosition(x + outerRadius + 1, y + radius),
        OpenGLPixelToPosition(x + radius, y + outerRadius + 1)
    };

    for (int i = 0; i < sizeof(upperLeftPos) / sizeof(upperLeftPos[0]); ++i)
    {
        glRectd(upperLeftPos[i].first,
                upperLeftPos[i].second,
                lowerRightPos[i].first,
                lowerRightPos[i].second);

    }

    // inner circle (white)
    glColor4d(1.0, 1.0, 1.0, 1.0);
    drawCircleOpenGL(x, y, getDiameter());
}
