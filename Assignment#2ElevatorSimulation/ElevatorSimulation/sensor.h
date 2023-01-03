#ifndef SENSOR_H
#define SENSOR_H

#include <QString>

class Sensor {
    private:
        const int WAIT_TIME = 5;
        int timeOnFloor;

    public:
        Sensor();

        const static QString arrivalMsg;
        const static QString departureMsg;

        bool waitingOnFloor;

        bool stillWaiting();
        void waitOnFloor();
        void leaveFloor();
        void arriveAtFloor();

        QString sensorInfo();
};

#endif // SENSOR_H
