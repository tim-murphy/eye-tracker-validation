// OpenGL helper functions

#ifndef OPENGLCOMMON_H
#define OPENGLCOMMON_H

#include <utility> // for std::pair

// Convert the pixel location to OpenGL relative position [-1.0, 1.0].
// note: pixel positions can be negative - we may want to draw a object which
//       is partially off screen
std::pair<double, double> OpenGLPixelToPosition(int pixelX, int pixelY);

#endif // not defined OPENGLCOMMON_H
