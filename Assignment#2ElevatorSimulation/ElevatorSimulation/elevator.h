#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "passenger.h"
#include "sensor.h"
#include "util.h"

#include <algorithm>
#include <QString>
#include <set>
#include <vector>


class Elevator {
    private:
        Direction     dir;
        std::set<int> travellingTo;

        Direction elevatorTravelDirection();

        void remPassengers(int floor);
        bool passengersDestination(int floor);

        QString elevatorInfo();
        QString showTravelInfo();
        QString showNumberOfPassengers();

    public:
        Elevator();

        Sensor sensor;

        int  current;
        bool isIdle;

        std::vector<Passenger> passengers;

        QString travel();
        void    travelToSafe();

        void addRequest(int floor);
        void addPassenger(Passenger p);

        bool willGoPast(int floor);

        QString buttonPressedInElevator(SpecialButton button);

};

#endif // ELEVATOR_H
