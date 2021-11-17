// Abstract class for storage of (x,y) pixel coordinates.
// Tim Murphy <tim@murphy.org> 2021

#include "ScreenPositionStore.h"

#include "common.h"

#include <iostream> // for operator<<

ScreenPositionStore::ScreenPositionStore(
    std::pair<unsigned int, unsigned int> right,
    std::pair<unsigned int, unsigned int> left,
    double id)
        : currentPosRight(right), currentPosLeft(left), identifier(id)
{}

std::pair<unsigned int, unsigned int>
    ScreenPositionStore::getCurrentPositionSingle() const
{
    // default values - will be overwritten if we have valid data
    std::pair<unsigned int, unsigned int> currentPos
        = std::make_pair(common::invalidCoord, common::invalidCoord);

    if (rightDataValid() && leftDataValid())
    {
        currentPos.first = (currentPosRight.first + currentPosLeft.first) / 2;
        currentPos.second = (currentPosRight.second + currentPosLeft.second) / 2;
    }
    else if (rightDataValid())
    {
        currentPos = currentPosRight;
    }
    else if (leftDataValid())
    {
        currentPos = currentPosLeft;
    }

    return currentPos;
}

std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> >
    ScreenPositionStore::getCurrentPositionRightLeft() const
{
    return std::make_pair(currentPosRight, currentPosLeft);
}

// for positions without right and left data, use the same value for both sides
void ScreenPositionStore::setCurrentPositionSingle(
    std::pair<unsigned int, unsigned int> pos, double id)
{
    setCurrentPositionRightLeft(pos, pos);
}

void ScreenPositionStore::setCurrentPositionRightLeft(
    std::pair<unsigned int, unsigned int> right,
    std::pair<unsigned int, unsigned int> left, double id)
{
    lock();
    currentPosRight = right;
    currentPosLeft = left;
    identifier = id;
    unlock();
}

double ScreenPositionStore::getIdentifier() const
{
    return identifier;
}

bool ScreenPositionStore::rightDataValid() const
{
    return (currentPosRight.first != common::invalidCoord &&
            currentPosRight.second != common::invalidCoord); 
}

bool ScreenPositionStore::leftDataValid() const
{
    return (currentPosLeft.first != common::invalidCoord &&
            currentPosLeft.second != common::invalidCoord);
}

std::ostream &operator<<(std::ostream &os,
                         const ScreenPositionStore &store)
{
    std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> > pos
        = store.getCurrentPositionRightLeft();
    os << "(R:" << pos.first.first << "," << pos.first.second
       << " L:" << pos.second.first << "," << pos.second.second << ")";
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
