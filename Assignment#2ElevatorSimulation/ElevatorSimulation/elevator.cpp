#include "elevator.h"

Elevator::Elevator() : dir(Direction::Up), current(randomInteger0to14()), isIdle(true)
{}

/******************************************************************************************/
/*
    Elevator travels to its highest floor then to its lowerst floor,
    this is the movement algorithm
*/
Direction Elevator::elevatorTravelDirection() {
    if (isIdle) return Direction::NotMoving;

    int highestFloor = *travellingTo.rbegin();
    int lowestFloor  = *travellingTo.begin();
    
    if (dir == Direction::Down && current <= lowestFloor)
        dir = Direction::Up;
    
    if (dir == Direction::Up && current >= highestFloor)
        dir = Direction::Down;

    return dir;
}

/******************************************************************************************/

void Elevator::addRequest(int floor) {
    travellingTo.insert(floor);
    isIdle = false;
}

/******************************************************************************************/

void Elevator::addPassenger(Passenger p) {
    if (p.current == p.destination) return; /* when the passenger is stupid and the current is their destination */

    isIdle = false;
  
    travellingTo.insert(p.destination);
    passengers.push_back(p);
}

/******************************************************************************************/

void Elevator::remPassengers(int floor) {
    auto result = std::remove_if(passengers.begin(), passengers.end(),
                                 [floor, this](const Passenger& p) {
                                    return (p.destination == floor);
                                 });

    passengers.erase(result, passengers.end());
}

/******************************************************************************************/

QString Elevator::travel() {
    if (isIdle) return "\nElevator is Idle";

    bool departing = false;

    if (sensor.waitingOnFloor) { /* If the Elevator is waiting on a floor */
        if (sensor.stillWaiting())
            return "Elevator Holding doors open at floor #" + QString::number(current + 1) + "\n"
                    + sensor.sensorInfo() + showTravelInfo() + showNumberOfPassengers();

        departing = true; /* Waiting is done so it must be departing */
    }

    current += elevatorTravelDirection();

    if (travellingTo.count(current)) { /* If the current floor is a destination of the elevator (told to travel to this floor)*/
        travellingTo.erase(current);

        if (passengersDestination(current)) /* If this is a passengers destination kicn them out */
            remPassengers(current);

        if (travellingTo.empty())
            isIdle = true;

        sensor.arriveAtFloor();
        return sensor.arrivalMsg;
    }

    return ((departing) ? sensor.departureMsg : "") + elevatorInfo();
}


/******************************************************************************************/

void Elevator::travelToSafe() {
    for (auto& p : passengers)
        p.destination = SAFE_FLOOR;

    travellingTo.clear();
    addRequest(SAFE_FLOOR);
}

/******************************************************************************************/
/* The Elevator will travel to all floors between its lowest destination & highest destination */
bool Elevator::willGoPast(int floor) {
    if (travellingTo.empty()) return false;

    int highestFloor = std::max(current, *travellingTo.rbegin());
    int lowestFloor  = std::min(current, *travellingTo.begin());

    return (floor >= lowestFloor && floor <= highestFloor);
}

/******************************************************************************************/

bool Elevator::passengersDestination(int floor) {
    auto isPassengerDestination = std::find_if(passengers.begin(), passengers.end(),
                                               [floor](const Passenger& p) {
                                                    return (p.destination == floor);
                                               });

    return (isPassengerDestination != passengers.end());
}

/******************************************************************************************/

QString Elevator::buttonPressedInElevator(SpecialButton button) {
    QString buttonPressed;

    if (button == SpecialButton::Hold) {
        buttonPressed += "\nA passenger pressed the HOLD button when entering\n";
        sensor.waitOnFloor();
    } else if (button == SpecialButton::Close) {
        buttonPressed += "\nA passenger pressed the CLOSE button when entering\n";
        sensor.leaveFloor();
    }

    return buttonPressed;
}

/******************************************************************************************/

QString Elevator::elevatorInfo() {
    return "Travelling to: " + QString::number(((dir == Direction::Up) ? *travellingTo.rbegin() + 1 : *travellingTo.begin() + 1)) + "th floor\n"
           + showNumberOfPassengers() + showTravelInfo();
}

/******************************************************************************************/

QString Elevator::showTravelInfo() {
    QString elevatorTravels = "Destinations floors { ";

    for (const int& n : travellingTo)
        elevatorTravels += QString::number(n + 1) + " ,";

    elevatorTravels.chop(1);
    elevatorTravels += "} \n";
    return elevatorTravels;
}

/******************************************************************************************/

QString Elevator::showNumberOfPassengers() {
    return  "The number of Passengers in the Elevator:  " + QString::number(passengers.size()) + "\n";
}
