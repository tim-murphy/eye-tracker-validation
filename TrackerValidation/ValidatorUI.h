// Abstract class for a graphical user interface for the validator system.
// Tim Murphy <tim@murphy.org> 2021

#ifndef VALIDATORUI_H
#define VALIDATORUI_H

#include <utility> // for std::pair

class ValidatorUI
{
  private:
    unsigned int targSize;
    static std::pair<unsigned int, unsigned int> screenRes;

    // calculate the screen resolution.
    static void setScreenRes();

  public:
    ValidatorUI(unsigned int targetSize);

    // display the target at the given (x,y) pixel location.
    virtual void showTarget(std::pair<unsigned int, unsigned int> pos) = 0;

    // set the idle routine (main processing)
    virtual void setIdleFunc(void (*func)(void)) = 0;

    // set the function used for mouse click events
    // this is the format for GLUT routines but should be flexible
    // enough for other UI systems if needed.
    virtual void setMouseFunc(void (*func)(int, int, int, int)) = 0;

    virtual void run() = 0;
    virtual void stop() = 0;

    // Are we currently running the test routine?
    // @ returns true if we are showing fixation targets
    // @ returns false if not ready to show targets (in menu, etc.)
    virtual bool inTestRoutine() const = 0;

    // -- getters and setters -- //
    unsigned int getTargetSize() const;
    static const std::pair<unsigned int, unsigned int> &getScreenRes();
};

# endif // defined VALIDATORUI_H