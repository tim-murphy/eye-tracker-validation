// Abstract class for a graphical user interface for the validator system.
// Tim Murphy <tim@murphy.org> 2021

#include "ValidatorUI.h"

#include <iostream>
#include <stdexcept>

// for setScreenRes
#ifdef _WIN32
    #include <wtypes.h>
#else
    #include <X11/Xlib.h>
#endif

ValidatorUI::ValidatorUI(unsigned int targetSize)
    :targSize(targetSize)
{
    setScreenRes();
}

void ValidatorUI::setScreenRes()
{
#ifdef _WIN32
    RECT screen;

    if (!GetWindowRect(GetDesktopWindow(), &screen))
    {
        throw std::runtime_error("Could not get screen resolution");
    }

    screenRes = std::make_pair(screen.right, screen.bottom);
#else
    Display *d = XOpenDisplay(nullptr);
    Screen *s = nullptr;

    if (d != nullptr)
    {
        s = DefaultScreenOfDisplay(d);
    }

    if (d == nullptr || s == nullptr)
    {
        throw std::runtime_error("Could not get screen resolution");
    }

    screenRes = std::make_pair(s->width, s->height);
#endif

    std::cout << "Screen resolution: " << screenRes.first << "x"
              << screenRes.second << std::endl;
}

unsigned int ValidatorUI::getTargetSize() const
{
    return targSize;
}

const std::pair<unsigned int, unsigned int> &ValidatorUI::getScreenRes()
{
    return screenRes;
}

std::pair<unsigned int, unsigned int> ValidatorUI::screenRes = getScreenRes();
