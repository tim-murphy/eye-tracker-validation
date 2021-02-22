// Circular fixation traget
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef CIRCLETARGET_H
#define CIRCLETARGET_H

#include "FixationTarget.h"

class CircleTarget : public FixationTarget
{
  private:
    // how many segments should be used to draw the circle?
    static const unsigned int segments = 32;

  public:
    CircleTarget(unsigned int diameter);

    // draw the target at pixel location (x, y)
    void drawOpenGL(unsigned int x, unsigned int y);
};

#endif // defined CIRCLETARGET_H