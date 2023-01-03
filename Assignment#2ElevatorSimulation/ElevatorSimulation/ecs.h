#ifndef ELEVATORCONTROLSYSTEM_H
#define ELEVATORCONTROLSYSTEM_H

#include <vector>

#include <QString>
#include <QDebug>
#include <QTimer>
#include <QMainWindow>
#include <QTextBrowser>


#include "util.h"
#include "passenger.h"
#include "elevator.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class ElevatorControlSystem;
}

QT_END_NAMESPACE

enum eMsg { Fire = 0, PowerOut = 1, Emergency = 2 };

class ElevatorControlSystem : public QMainWindow {
    Q_OBJECT

    public:
        ElevatorControlSystem(QWidget* parent = nullptr);
        ~ElevatorControlSystem();

    private slots:
        void runSimulation();
        void emergencyButtonsClicked();

    private:
        Ui::ElevatorControlSystem *ui;

        const int NUM_FLOORS    = 15;
        const int NUM_ELEVATORS = 3;
        const int SIM_SPEED     = 3000;

        bool fireEmergency;
        bool powerOutEmergency;
        bool helpRequested;
        bool informElevators;

        QTimer* timer;

        std::vector<std::vector<Passenger>> building;

        std::vector<bool>     floorReq;
        std::vector<Elevator> elevators;
        std::vector<QString>  emergencyMessages;

        std::vector<QTextBrowser*> floorDisplays;

        void runSafeMode();

        int  createPassenger();
        void pickupPassengers();
        void allocateElevators(int floor);

        void          editElevatorDisplay(int elevator, QString msg);
        QTextBrowser* getElevatorDisplay(int elevator);

        void          editFloorDisplay(int floor, QString msg, bool clearDispay = true);
        QTextBrowser* getFloorDisplay(int floor);
        void          displayFloorCounts();
};

#endif // ELEVATORCONTROLSYSTEM_H
