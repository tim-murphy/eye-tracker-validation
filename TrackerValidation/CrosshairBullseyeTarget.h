// Bullseye target (large circle with small inner circle) with crosshairs
// Noted as the most stable fixation target compared to others:
// https://www.sciencedirect.com/science/article/pii/S0042698912003380
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef CROSSHAIRBULLSEYETARGET_H
#define CROSSHAIRBULLSEYETARGET_H

#include "FixationTarget.h"

class CrosshairBullseyeTarget : public FixationTarget
{
private:
    // how many segments should be used to draw each circle?
    static const unsigned int segments = 128;

    // diameter of the outer circle of the target. The paper mentioned above
    // uses a ratio of 3:1
    static const unsigned int targetRatio = 3;
    unsigned int getOuterDiameter(void) const;

    // helper function
    static void drawCircle(unsigned int x, unsigned int y,
                           unsigned int diameter);

  public:
    // diameter in this case refers to the size of the inner circle of the
    // target, and the width of the crosshair lines
    CrosshairBullseyeTarget(unsigned int diameter);

    // draw the target at pixel location (x, y)
    void drawOpenGL(unsigned int x, unsigned int y);
};

#endif // not defined CROSSHAIRBULLSEYETARGET_H