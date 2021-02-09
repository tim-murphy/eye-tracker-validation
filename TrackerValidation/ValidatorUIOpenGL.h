// OpenGL implementation of the validator UI
// Tim Murphy <tim@murphy.org> 2021

#ifndef VALIDATORUIOPENGL_H
#define VALIDATORUIOPENGL_H

#include "ValidatorUI.h"

class ValidatorUIOpenGL : public ValidatorUI
{
  private:
    // UI callbacks
    static bool fullscreen;
    static void drawScreen();
    static void keypress(unsigned char key, int x, int y);
    static void resize(int width, int height);
    static void showSplashScreen();

    // convert pixel location into OpenGL relative position [-1.0, 1.0]
    static std::pair<double, double> pixelToPosition(unsigned int x,
                                                     unsigned int y);

    // draw a circle at pixel location (x, y) with the given radius
    // using trianges.
    // @param segments number of trianges to use to draw the circle
    static void drawCircle(unsigned int x, unsigned int y,
                           double diameter, unsigned int segments=32);

  public:
    ValidatorUIOpenGL(unsigned int targetSize, int *argcp, char **argvp);

    // set the idle routine (main processing)
    void setIdleFunc(void (*func)(void));

    // set the mouse click handler routine
    void setMouseFunc(void (*func)(int, int, int, int));

    void run();
    void stop();

    void showTarget(std::pair<unsigned int, unsigned int> pos);

    bool inTestRoutine() const;

    // mouseclick event helpers - check if button/state combination is
    // considered a valid mouse click
    static bool mouseClickEvent(int button, int state);
};

# endif // defined VALIDATORUIOPENGL_H