// OpenGL helper functions

#ifndef OPENGLCOMMON_H
#define OPENGLCOMMON_H

#include <utility> // for std::pair

// Convert the pixel location to OpenGL relative position [-1.0, 1.0].
std::pair<double, double> OpenGLPixelToPosition(
    unsigned int pixelX, unsigned int pixelY);

#endif // not defined OPENGLCOMMON_H
