#include "sensor.h"

const QString Sensor::arrivalMsg   = "\n** BELL RINGS! Doors open **\n ";
const QString Sensor::departureMsg = "\n** BELL RINGS! Doors close **\n";

/******************************************************************************************/

Sensor::Sensor() : timeOnFloor(0), waitingOnFloor(false){}

/******************************************************************************************/

void Sensor::waitOnFloor()  { timeOnFloor = WAIT_TIME; }

/******************************************************************************************/

void Sensor::leaveFloor() { timeOnFloor = 0; }

/******************************************************************************************/

bool Sensor::stillWaiting() {
    timeOnFloor--;
    waitingOnFloor = (timeOnFloor <= 0) ? false : true;

    return waitingOnFloor;
}

/******************************************************************************************/

void Sensor::arriveAtFloor() {
    if (waitingOnFloor) return;

    waitingOnFloor = true;
    waitOnFloor();
}

/******************************************************************************************/

QString Sensor::sensorInfo() {
    return "\nSensor: Waiting on Floor for " + QString::number(timeOnFloor) + " seconds\n";
}
