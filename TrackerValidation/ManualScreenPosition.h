// Store for screen co-ordinates which cannot be calculated
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef MANUALSCREENPOSITION_H
#define MANUALSCREENPOSITION_H

#include "ScreenPositionStore.h"

class ManualScreenPosition : public ScreenPositionStore
{
  private:
    void setCurrentPosition(); // not allowed for this implementation.

  public:
    ManualScreenPosition();
};

#endif // defined MANUALSCREENPOSITION_H