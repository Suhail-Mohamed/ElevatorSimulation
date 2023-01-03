#ifndef PASSENGER_H
#define PASSENGER_H

#include "util.h"

class Passenger {
    public:
        Passenger();

        int current;
        int destination;

        bool coveringSensor;

        SpecialButton enteringElevator();
};

#endif // PASSENGER_H
