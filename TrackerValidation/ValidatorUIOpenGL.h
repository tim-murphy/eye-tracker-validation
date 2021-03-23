// OpenGL implementation of the validator UI
// This has been implemented as a singleton as it is using a C library and
// things get messy quickly with a mixture of static and non-static functions.
// Tim Murphy <tim@murphy.org> 2021

#ifndef VALIDATORUIOPENGL_H
#define VALIDATORUIOPENGL_H

#include "ValidatorUI.h"

#include <mutex>

class ValidatorUIOpenGL : public ValidatorUI
{
  private:
    // singleton requirements
    static ValidatorUIOpenGL *inst;
    static std::mutex createLock;

    // UI callbacks
    // Some of these need to be static as they are passed to OpenGL using the
    // C library
    bool fullscreen;
    static void drawScreen();
    static void keypress(unsigned char key, int x, int y);
    static void resize(int width, int height);
    void showSplashScreen();

    std::pair<unsigned int, unsigned int> currTargetPos;

    // draw a target at pixel location (x, y) with the given radius.
    void drawTarget(unsigned int x, unsigned int y,
                    unsigned int diameter);

    // private constructor as this is a factory design
    ValidatorUIOpenGL(unsigned int targetSize, const std::string &targType,
                      int *argcp, char **argvp, bool previewMode);
  public:
    static ValidatorUIOpenGL *create(unsigned int targetSize,
                                     const std::string &targType,
                                     int *argcp, char **argvp,
                                     bool previewMode = false);

    // returns nullptr if this has not yet been created
    static ValidatorUIOpenGL *getInstance();

    // set the idle routine (main processing)
    void setIdleFunc(void (*func)(void));

    // set the mouse click handler routine
    void setMouseFunc(void (*func)(int, int, int, int));

    void run();
    void stop();

    void showTarget(std::pair<unsigned int, unsigned int> pos,
                    bool drawScreen = true);

    bool inTestRoutine() const;

    // mouseclick event helpers - check if button/state combination is
    // considered a valid mouse click
    bool mouseClickEvent(int button, int state);

    // convert pixel location into OpenGL relative position [-1.0, 1.0]
    static std::pair<double, double> pixelToPosition(
        unsigned int x, unsigned int y);
};

# endif // defined VALIDATORUIOPENGL_H
