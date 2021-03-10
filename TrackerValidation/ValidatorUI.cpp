// Abstract class for a graphical user interface for the validator system.
// Tim Murphy <tim@murphy.org> 2021

#include "ValidatorUI.h"

ValidatorUI::ValidatorUI(unsigned int targetSize,
                         const std::string &targetType)
    :targSize(targetSize), targType(targetType)
{ }

unsigned int ValidatorUI::getTargetSize() const
{
    return targSize;
}

const std::string &ValidatorUI::getTargetType() const
{
    return targType;
}
