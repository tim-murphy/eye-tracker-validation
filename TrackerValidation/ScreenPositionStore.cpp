// Abstract class for storage of (x,y) pixel coordinates.
// Tim Murphy <tim@murphy.org> 2021

#include "ScreenPositionStore.h"

std::pair<unsigned int, unsigned int>
    ScreenPositionStore::getCurrentPosition() const
{
    return currentPos;
}

void ScreenPositionStore::setCurrentPosition(
    std::pair<unsigned int, unsigned int> pos, double id)
{
    currentPos = pos;
    identifier = id;
}

double ScreenPositionStore::getIdentifier() const
{
    return identifier;
}