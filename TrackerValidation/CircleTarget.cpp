// Circular fixation target
// Written by Tim Murphy <tim@murphy.org> 2021

#include "CircleTarget.h"

#include "common.h"
#include "ValidatorUIOpenGL.h"

#include <cmath>
#include <GL/freeglut.h>
#include <utility>

CircleTarget::CircleTarget(unsigned int diameter)
    : FixationTarget(diameter)
{}

void CircleTarget::drawOpenGL(unsigned int x, unsigned int y)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor4d(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLE_FAN);

    double radius = static_cast<double>(getDiameter()) / 2.0;

    std::pair<double, double> centerPos = ValidatorUIOpenGL::pixelToPosition(x, y);
    glVertex2d(centerPos.first, centerPos.second);

    for (double n = 0; n <= segments; ++n)
    {
        // the angle used for this triangle segment
        double sigma = n * common::pi * 2.0 / static_cast<double>(segments);

        std::pair<double, double> pos = ValidatorUIOpenGL::pixelToPosition(
            static_cast<unsigned int>(x + (radius * cos(sigma))),
            static_cast<unsigned int>(y + (radius * sin(sigma))));

        glVertex2d(pos.first, pos.second);
    }

    glEnd();
}