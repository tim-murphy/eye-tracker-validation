// Abstract class for a graphical user interface for the validator system.
// Tim Murphy <tim@murphy.org> 2021

#ifndef VALIDATORUI_H
#define VALIDATORUI_H

#include <string>
#include <utility> // for std::pair

class ValidatorUI
{
  private:
    unsigned int targSize;
    std::string targType;
    bool preview;

  public:
    ValidatorUI(unsigned int targetSize,
                const std::string &targetType,
                bool previewMode = false);

    // display the target at the given (x,y) pixel location.
    // @param pos (x, y) pixel co-ordinates for the center of the target
    // @param drawScreen display target to screen (if drawing multiple targets,
    //                   set this to false for all except the last call)
    // @param firstTarget is this the first target to display for this set?
    //                    (always true if only showing one target at a time)
    virtual void showTarget(std::pair<unsigned int, unsigned int> pos,
                            bool drawScreen = true,
                            bool firstTarget = true) = 0;

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
    const std::string &getTargetType() const;
    bool inPreviewMode() const;
};

# endif // defined VALIDATORUI_H