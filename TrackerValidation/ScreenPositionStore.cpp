// Abstract class for storage of (x,y) pixel coordinates.
// Tim Murphy <tim@murphy.org> 2021

#include "ScreenPositionStore.h"

#include <iostream> // for operator<<

ScreenPositionStore::ScreenPositionStore(std::pair<unsigned int, unsigned int> pos,
                                         double id)
    : currentPos(pos), identifier(id)
{}

std::pair<unsigned int, unsigned int>
    ScreenPositionStore::getCurrentPosition() const
{
    return currentPos;
}

void ScreenPositionStore::setCurrentPosition(
    std::pair<unsigned int, unsigned int> pos, double id)
{
    lock();
    currentPos = pos;
    identifier = id;
    unlock();
}

double ScreenPositionStore::getIdentifier() const
{
    return identifier;
}

std::ostream &operator<<(std::ostream &os,
                         const ScreenPositionStore &store)
{
    std::pair<unsigned int, unsigned int> pos = store.getCurrentPosition();
    os << "(" << pos.first << "," << pos.second << ")";
    return os;
}

void ScreenPositionStore::lock(void)
{
    posMutex.lock();
}

void ScreenPositionStore::unlock(void)
{
    posMutex.unlock();
}
