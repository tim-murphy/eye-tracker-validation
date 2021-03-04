// Fixation target abstract class
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef FIXATIONTARGET_H
#define FIXATIONTARGET_H

#include <string>

class FixationTarget
{
  private:
    // Diameter of the target, in pixels.
    unsigned int diameter;

  public:
    FixationTarget(unsigned int diameter);

    // draw target with OpenGL at pixel location (x, y)
    virtual void drawOpenGL(unsigned int x, unsigned int y) = 0;

    // create a target of the given type.
    // @throws std::runtime_error if type does not match a known type.
    static FixationTarget *create(const std::string &type,
                                  unsigned int diameter);

    // getters and setters
    unsigned int getDiameter(void) const;
};

#endif // not defined FIXATIONTARGET_H