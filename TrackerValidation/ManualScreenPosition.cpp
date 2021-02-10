// Store for screen co-ordinates which cannot be calculated
// Written by Tim Murphy <tim@murphy.org> 2021

#include "ManualScreenPosition.h"

#include <stdexcept>

ManualScreenPosition::ManualScreenPosition()
    : ScreenPositionStore()
{
    ScreenPositionStore::setCurrentPosition(std::make_pair(0, 0));
}

void ManualScreenPosition::setCurrentPosition()
{
    throw std::runtime_error("Screen position cannot be auto-calculated.");
}
