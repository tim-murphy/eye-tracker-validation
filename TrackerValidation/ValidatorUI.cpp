// Abstract class for a graphical user interface for the validator system.
// Tim Murphy <tim@murphy.org> 2021

#include "ValidatorUI.h"

#include "common.h"

#include <iostream>
#include <stdexcept>

ValidatorUI::ValidatorUI(unsigned int targetSize,
                         const std::string &targetType)
    :targSize(targetSize), targType(targetType)
{
    setScreenRes();
}

void ValidatorUI::setScreenRes()
{
    screenRes = common::getScreenRes();
    std::cout << "Screen resolution: " << screenRes.first << "x"
              << screenRes.second << std::endl;
}

unsigned int ValidatorUI::getTargetSize() const
{
    return targSize;
}

const std::string &ValidatorUI::getTargetType() const
{
    return targType;
}

const std::pair<unsigned int, unsigned int> &ValidatorUI::getScreenRes()
{
    return screenRes;
}

std::pair<unsigned int, unsigned int> ValidatorUI::screenRes = getScreenRes();
