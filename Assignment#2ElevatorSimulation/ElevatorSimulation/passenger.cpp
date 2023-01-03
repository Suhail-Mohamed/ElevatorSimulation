#include "passenger.h"

/******************************************************************************************/

Passenger::Passenger()
{
    current        = randomInteger0to14();
    destination    = randomInteger0to14();
    coveringSensor = (randomInteger0to14() >= 11) ? true : false;
}

/******************************************************************************************/

SpecialButton Passenger::enteringElevator() {
    int randomVal = randomInteger0to14();

    if      (randomVal >= 4 && randomVal <= 6)  return SpecialButton::Hold;
    else if (randomVal >= 7 && randomVal <= 10) return SpecialButton::Close;

    return SpecialButton::NoButtonClicked;
}
