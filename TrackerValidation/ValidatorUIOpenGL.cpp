// OpenGL implementation of the validator UI
// Tim Murphy <tim@murphy.org> 2021

#include "ValidatorUIOpenGL.h"

#include "common.h"
#include "FixationTarget.h"

#include <GL/freeglut.h>
#include <iostream>
#include <memory>
#include <thread>

// -- singleton initialisation -- //
ValidatorUIOpenGL *ValidatorUIOpenGL::create(unsigned int targetSize,
                                             const std::string &targType,
                                             int *argcp, char **argvp)
{
    const std::lock_guard<std::mutex> lock(createLock);
    if (inst == nullptr)
    {
        inst = new ValidatorUIOpenGL(targetSize, targType, argcp, argvp);
    }

    return inst;
}

ValidatorUIOpenGL *ValidatorUIOpenGL::getInstance()
{
    return inst;
}

// -- UI static functions -- //
void ValidatorUIOpenGL::drawScreen()
{
    ValidatorUIOpenGL * const ui = ValidatorUIOpenGL::getInstance();

    // this should never happen
    if (ui == nullptr)
    {
        throw std::runtime_error("drawScreen() called before UI was started!");
    }

    // redraw the screen
    ui->showTarget(ui->currTargetPos);
}

void ValidatorUIOpenGL::keypress(unsigned char key, int, int)
{
    ValidatorUIOpenGL * const ui = ValidatorUIOpenGL::getInstance();

    // this should never happen
    if (ui == nullptr)
    {
        throw std::runtime_error("keypress called before UI was started!");
    }

    switch (key)
    {
      case 27: // escape key
        std::cout << "Shutting down the test." << std::endl;
        glutLeaveMainLoop();
        break;

      case 32:
        if (!ui->fullscreen)
        {
            std::cout << "Changing to full-screen mode" << std::endl;
            std::pair<unsigned int, unsigned int> res = common::getScreenRes();
            glutReshapeWindow(res.first, res.second);
            glutPositionWindow(0, 0);
            glutFullScreen();
            ui->fullscreen = true;
        }
        break;
    }
}
void ValidatorUIOpenGL::resize(int width, int height)
{
    ValidatorUIOpenGL * const ui = ValidatorUIOpenGL::getInstance();

    // this should never happen
    if (ui == nullptr)
    {
        throw std::runtime_error("resize called before UI was started!");
    }

    glViewport(0, 0, width, height);

    if (!ui->fullscreen)
    {
        ui->showSplashScreen();
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
    currTargetPos = std::make_pair(x, y);
}

// -- end UI static functions --//

ValidatorUIOpenGL::ValidatorUIOpenGL(unsigned int targetSize,
                                     const std::string &targetType,
                                     int *argcp, char **argvp)
    : ValidatorUI(targetSize, targetType),
      fullscreen(false), currTargetPos(std::make_pair(0,0))
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

std::mutex ValidatorUIOpenGL::createLock;
ValidatorUIOpenGL *ValidatorUIOpenGL::inst{nullptr};
