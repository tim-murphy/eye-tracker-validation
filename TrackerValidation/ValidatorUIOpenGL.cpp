// OpenGL implementation of the validator UI
// Tim Murphy <tim@murphy.org> 2021

#include "ValidatorUIOpenGL.h"

#include "common.h"
#include "FixationTarget.h"

#include <GL/freeglut.h>
#include <iostream>
#include <memory>
#include <thread>

// -- UI static functions -- //
void ValidatorUIOpenGL::drawScreen()
{
    // nothing to do here
}

bool ValidatorUIOpenGL::fullscreen = false;
void ValidatorUIOpenGL::keypress(unsigned char key, int, int)
{
    switch (key)
    {
      case 27: // escape key
        std::cout << "Shutting down the test." << std::endl;
        glutLeaveMainLoop();
        break;

      case 32:
        if (!fullscreen)
        {
            std::cout << "Changing to full-screen mode" << std::endl;
            glutReshapeWindow(getScreenRes().first, getScreenRes().second);
            glutPositionWindow(0, 0);
            glutFullScreen();
            glutSwapBuffers();
            fullscreen = true;
        }
        break;
    }
}
void ValidatorUIOpenGL::resize(int width, int height)
{
    glViewport(0, 0, width, height);

    if (!fullscreen)
    {
        showSplashScreen();
    }
}

void ValidatorUIOpenGL::showSplashScreen()
{
    static void *font = GLUT_BITMAP_HELVETICA_12;
    static unsigned char instructions[] = "Press the space bar to begin.\r\n"
                                          "To exit, press ESC at any time.";

    // center the font on the screen
    glRasterPos2d(0.5 - static_cast<double>(glutBitmapLength(font, instructions)),
                  0.5 - static_cast<double>(glutBitmapHeight(font)));
    glRasterPos2d(-0.5, 0.0);
    glColor4d(1.0, 1.0, 1.0, 1.0);

    //draw to screen
    for (unsigned char *c = instructions; *c != '\0'; ++c)
    {
        glutBitmapCharacter(font, *c);
    }

    glutSwapBuffers();
}

bool ValidatorUIOpenGL::mouseClickEvent(int button, int state)
{
    if (!fullscreen)
    {
        // we've clicked the mouse at the splash screen - ignore it
        return false;
    }

    return (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN);
}

void ValidatorUIOpenGL::drawTarget(unsigned int x, unsigned int y,
                                   unsigned int diameter)
{
    std::unique_ptr<FixationTarget> t(
        FixationTarget::create(getTargetType(), diameter));
    t->drawOpenGL(x, y);
}
std::pair<double, double> ValidatorUIOpenGL::pixelToPosition(unsigned int x,
                                                             unsigned int y)
{
    // Convert the pixel locations into relative positions [-1.0, 1.0].
    // Can be done by 2 x (actual_pixel_value / max_pixel_value) - 1
    // for each axis.
    // e.g. location (100, 100) on a 1024x768 screen would be:
    //  x = 2 x (100/1024) - 1 = -0.80
    //  y = 1 - (2 x (100/768) = 0.74
    //  So returned value would be (-0.80, 0.74)
    std::pair<unsigned int, unsigned int> res = getScreenRes();
    double xPos = 2 * (static_cast<double>(x)
                  / static_cast<double>(res.first)) - 1.0;
    double yPos = 1.0 - (2 * (static_cast<double>(y)
                  / static_cast<double>(res.second)));

    return std::make_pair(xPos, yPos);
}

// -- end UI static functions --//

ValidatorUIOpenGL::ValidatorUIOpenGL(unsigned int targetSize,
                                     const std::string &targetType,
                                     int *argcp, char **argvp)
    : ValidatorUI(targetSize, targetType)
{
    static constexpr std::pair<int, int> windowRes = std::make_pair(640, 480);

    // Initialise OpenGL
    glutInit(argcp, argvp);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
                  GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // double buffering
    glutInitWindowSize(windowRes.first, windowRes.second);
    glutCreateWindow("Eye Tracker Validator");
    glutDisplayFunc(this->drawScreen);
    glutKeyboardFunc(this->keypress);
    glutReshapeFunc(this->resize);
    showSplashScreen();
}

void ValidatorUIOpenGL::setIdleFunc(void (*func)(void))
{
    glutIdleFunc(func);
}

void ValidatorUIOpenGL::setMouseFunc(void (*func)(int, int, int, int))
{
    glutMouseFunc(func);
}

void ValidatorUIOpenGL::run()
{
    glutMainLoop();
}

void ValidatorUIOpenGL::stop()
{
    glutLeaveMainLoop();
}

void ValidatorUIOpenGL::showTarget(std::pair<unsigned int, unsigned int> pos)
{
    if (!inTestRoutine())
    {
        return;
    }

    drawTarget(pos.first, pos.second, getTargetSize());

    glutSwapBuffers();
}

bool ValidatorUIOpenGL::inTestRoutine() const
{
    // if we're fullscreen then we're testing.
    return fullscreen;
}