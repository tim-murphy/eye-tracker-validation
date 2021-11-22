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
                                             int *argcp, char **argvp,
                                             bool previewMode)
{
    const std::lock_guard<std::mutex> lock(createLock);
    if (inst == nullptr)
    {
        inst = new ValidatorUIOpenGL(targetSize, targType, argcp, argvp,
                                     previewMode);
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

    constexpr int numWindows = sizeof(ui->displayWindows) / sizeof(ui->displayWindows[0]);
    for (int x = 0; x < numWindows; ++x)
    {
        // redraw the screen
        glutSetWindow(ui->displayWindows[x]);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!ui->inTestRoutine())
        {
            ui->showSplashScreen();
        }
        else
        {
            for (auto pos : ui->currTargetPos)
            {
                ui->drawTarget(pos.first, pos.second, ui->getTargetSize());
            }

            // show our gaze positions as well on the monitor window
            if (x != 0)
            {
                ui->drawFixation(ui->posRight.first, ui->posRight.second);
                ui->drawFixation(ui->posLeft.first, ui->posLeft.second);
            }
        }
        glutSwapBuffers();
    }
    glutSetWindow(ui->displayWindows[0]);
}

// screen drawing is being handled by the main drawScreen method
void ValidatorUIOpenGL::drawScreenMonitor()
{}

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
        ui->stop();
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

      default:
        glutPostRedisplay();
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
}

bool ValidatorUIOpenGL::mouseClickEvent(int button, int state)
{
    // ignore clicks on the splash screen or in preview mode
    if (!fullscreen || inPreviewMode())
    {
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

void ValidatorUIOpenGL::drawFixation(unsigned int x, unsigned int y)
{
    std::pair<unsigned int, unsigned int> res = common::getScreenRes();

    if (x <= res.first && y <= res.second)
    {
        std::unique_ptr<FixationTarget> t(
            FixationTarget::create("circle", 10));
        t->drawOpenGL(x, y);
    }
}

void ValidatorUIOpenGL::setGazePos(
    std::pair<unsigned int, unsigned int> r,
    std::pair<unsigned int, unsigned int> l)
{
    posRight = r;
    posLeft = l;
}

// -- end UI static functions --//

ValidatorUIOpenGL::ValidatorUIOpenGL(unsigned int targetSize,
                                     const std::string &targetType,
                                     int *argcp, char **argvp,
                                     bool previewMode)
    : ValidatorUI(targetSize, targetType, previewMode),
      fullscreen(false), currTargetPos()
{
    static constexpr std::pair<int, int> windowRes = std::make_pair(640, 480);

    // Initialise OpenGL
    glutInit(argcp, argvp);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
                  GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // double buffering
    glutInitWindowSize(windowRes.first, windowRes.second);
    displayWindows[0] = glutCreateWindow("Eye Tracker Validator");
    glutKeyboardFunc(this->keypress);
    glutDisplayFunc(this->drawScreen);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // double buffering
    glutInitWindowSize(windowRes.first, windowRes.second);
    displayWindows[1] = glutCreateWindow("Eye Tracker Validator :: Monitor");
    glutDisplayFunc(this->drawScreenMonitor);

    constexpr int numWindows = sizeof(displayWindows) / sizeof(displayWindows[0]);
    for (int x = 0; x < numWindows; ++x)
    {
        glutSetWindow(displayWindows[x]);
        glutReshapeFunc(this->resize);
    }
    glutSetWindow(displayWindows[0]);
    showSplashScreen();
    glutPostRedisplay();
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
    fullscreen = false;
    glutLeaveMainLoop();
}

void ValidatorUIOpenGL::refresh()
{
    if (inTestRoutine())
    {
        glutPostRedisplay();
    }
}

void ValidatorUIOpenGL::showTarget(std::pair<unsigned int, unsigned int> pos,
                                   bool drawScreen, bool firstTarget)
{
    if (!inTestRoutine())
    {
        return;
    }

    if (!inPreviewMode())
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    if (firstTarget)
    {
        currTargetPos.clear();
    }
    
    currTargetPos.push_back(std::make_pair(pos.first, pos.second));

    drawTarget(pos.first, pos.second, getTargetSize());

    if (drawScreen)
    {
        glutPostRedisplay();
    }
}

bool ValidatorUIOpenGL::inTestRoutine() const
{
    // if we're fullscreen then we're testing.
    return fullscreen;
}

std::mutex ValidatorUIOpenGL::createLock;
ValidatorUIOpenGL *ValidatorUIOpenGL::inst{nullptr};
