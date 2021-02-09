// Accessor for the current cursor position.
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef CURSORPOSITION_H
#define CURSORPOSITION_H

#include "ScreenPositionStore.h"

class CursorPosition
    : public ScreenPositionStore
{
  public:
    void setCurrentPosition();
};

#endif // defined CURSORPOSITION_H