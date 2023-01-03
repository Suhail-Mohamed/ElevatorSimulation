#include "ecs.h"
#include "ui_elevatorcontrolsystem.h"

ElevatorControlSystem::ElevatorControlSystem(QWidget *parent)
    : QMainWindow        (parent)
    , ui                 (new Ui::ElevatorControlSystem)
    , fireEmergency      (false)
    , powerOutEmergency  (false)
    , helpRequested(false)
    , informElevators    (false)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    elevators = std::vector<Elevator>               (NUM_ELEVATORS);
    floorReq  = std::vector<bool>                   (NUM_FLOORS, false);
    building  = std::vector<std::vector<Passenger>> (NUM_FLOORS);

    floorDisplays = std::vector<QTextBrowser*> {
        ui->floor1 , ui->floor2 , ui->floor3 , ui->floor4 , ui->floor5 ,
        ui->floor6 , ui->floor7 , ui->floor8 , ui->floor9 , ui->floor10,
        ui->floor11, ui->floor12, ui->floor13, ui->floor14, ui->floor15
    };

    emergencyMessages = std::vector<QString> { "\nFIRE IN BUILDING", "\nPOWEROUT IN BUILDING", "\nHELP REQUESTED, System Reset" };

    connect(timer, SIGNAL(timeout()), this, SLOT(runSimulation()));

    connect(ui->elevator1Help , SIGNAL(clicked()), this, SLOT(emergencyButtonsClicked()));
    connect(ui->elevator2Help , SIGNAL(clicked()), this, SLOT(emergencyButtonsClicked()));
    connect(ui->elevator3Help , SIGNAL(clicked()), this, SLOT(emergencyButtonsClicked()));
    connect(ui->fireButton    , SIGNAL(clicked()), this, SLOT(emergencyButtonsClicked()));
    connect(ui->powerOutButton, SIGNAL(clicked()), this, SLOT(emergencyButtonsClicked()));

    timer->start(SIM_SPEED);
    displayFloorCounts();
}

/******************************************************************************************/

ElevatorControlSystem::~ElevatorControlSystem() {
    delete ui;
}

/******************************************************************************************/

void ElevatorControlSystem::runSimulation() {
    if (ui->pauseSimBox->isChecked()) return;

    if (fireEmergency || powerOutEmergency || helpRequested) {
        runSafeMode();
        return;
    }

    for (int eIdx = 0; eIdx < NUM_ELEVATORS; ++eIdx)
        editElevatorDisplay(eIdx, elevators[eIdx].travel());

    int clicked = createPassenger();
    displayFloorCounts();
    editFloorDisplay(abs(clicked), ((clicked < 0) ? "Clicked Down" : "Clicked Up"), false);

    pickupPassengers();
    return;
}

/******************************************************************************************/
void ElevatorControlSystem::runSafeMode() {
    eMsg m;
    int  allOnSafeFloor = 0;

    if (powerOutEmergency) m = eMsg::PowerOut;
    if (helpRequested)     m = eMsg::Emergency;
    if (fireEmergency)     m = eMsg::Fire;

    if (!informElevators) {
        for (auto& floor : building)
            floor.clear();

        for (auto& e : elevators)
            e.travelToSafe();

        informElevators = true;
    }

    for (int eIdx = 0; eIdx < NUM_ELEVATORS; ++eIdx) {
        editElevatorDisplay(eIdx, elevators[eIdx].travel() + emergencyMessages[m]);

        allOnSafeFloor += (elevators[eIdx].current == SAFE_FLOOR);
    }

    if (allOnSafeFloor == NUM_ELEVATORS)
        informElevators = powerOutEmergency = fireEmergency = helpRequested = false;

     displayFloorCounts();
}

/******************************************************************************************/

int ElevatorControlSystem::createPassenger() {
    Passenger p = Passenger();

    if (!floorReq[p.current]) {
        floorReq[p.current] = true;
        allocateElevators(p.current);
    }

    building[p.current].push_back(p);

    return (p.current > p.destination) ? p.current * Direction::Down : p.current * Direction::Up;
}

/******************************************************************************************/

void ElevatorControlSystem::pickupPassengers() {
    for (int eIdx = 0; eIdx < NUM_ELEVATORS; ++eIdx) {
        auto& e = elevators[eIdx];

        if (floorReq[e.current]) {
            e.sensor.arriveAtFloor(); /* Wait on this floor as you are picking up some passengers */

            auto& floorPassengers = building[e.current];
            auto findPassengerCovering = std::find_if(floorPassengers.begin(), floorPassengers.end(),
                                                      [](const Passenger& p ) { return p.coveringSensor; });

            /* Passenger covering elevator, send message to system and then add passengers to elevator */
            if (findPassengerCovering != floorPassengers.end())
                editElevatorDisplay(eIdx, "\nPassenger Covering Elevator Door, Please move out of way");

            /*
               Add passengers to elevator from the floor as passengers have moved obstacles out of way
               Passengers click a button when they enter the elevator like hold or close
            */
            while (!floorPassengers.empty()) {
                auto& curPass       = floorPassengers.back();
                auto  buttonClicked = curPass.enteringElevator();
                auto  message       = e.buttonPressedInElevator(buttonClicked);

                e.addPassenger(curPass);

                if (message.size() > 0)
                    editElevatorDisplay(eIdx, message);

                floorPassengers.pop_back();
            }

            floorReq[e.current] = false;
        }
    }
}

/******************************************************************************************/

void ElevatorControlSystem::allocateElevators(int floor) {
    /* Find an idle elevator to use */
    auto idleElevator = std::find_if(elevators.begin(), elevators.end(),
                                     [](const Elevator& e) { return e.isIdle; });

    if (idleElevator != elevators.end()) {
        idleElevator->addRequest(floor);
        return;
    }

    /* See if an elevator will past by the floor, if there is one then return  */
    auto elevatorAlreadyAllocated = std::find_if(elevators.begin(), elevators.end(),
                                                 [floor](Elevator& e) { return e.willGoPast(floor); });

    if (elevatorAlreadyAllocated != elevators.end())
        return;

    /* No elevators passing by floor make the least filled one go to floor */
    auto leastBusy = std::min_element(elevators.begin(), elevators.end(),
                                      [floor](const Elevator& lhs, const Elevator& rhs) {
                                              return (lhs.passengers.size() < rhs.passengers.size() );
                                      });

    leastBusy->addRequest(floor);
}

/******************************************************************************************/

QTextBrowser* ElevatorControlSystem::getElevatorDisplay(int elevator) {
    switch (elevator) {
        case 0  : return ui->elevator1;
        case 1  : return ui->elevator2;
        case 2  : return ui->elevator3;
        default : throw "Illegal Index Access";
    }
}

/******************************************************************************************/

void ElevatorControlSystem::editElevatorDisplay(int elevator, QString msg) {
    auto display = getElevatorDisplay(elevator);

    display->clear();
    display->append("Elevator#" + QString::number(elevator + 1)
                    + ": (At Floor: " + QString::number(elevators[elevator].current + 1) +") \n" +  msg);
}

/******************************************************************************************/

QTextBrowser* ElevatorControlSystem::getFloorDisplay(int floor) {
    return floorDisplays[floor];
}

/******************************************************************************************/

void ElevatorControlSystem::editFloorDisplay(int floor, QString msg, bool clearDisplay) {
    auto display = getFloorDisplay(floor);

    if (clearDisplay) display->clear();
    display->append("Floor#" + QString::number(floor + 1) + ": " + msg);
}

/******************************************************************************************/

void ElevatorControlSystem::displayFloorCounts() {
    for (int floor = 0; floor < NUM_FLOORS; ++floor)
        editFloorDisplay(floor, "\n# of Passengers = " + QString::number(building[floor].size()));
}

/******************************************************************************************/

void ElevatorControlSystem::emergencyButtonsClicked() {
    if (helpRequested || fireEmergency || powerOutEmergency) return;

    QPushButton* buttonPressed = qobject_cast<QPushButton*>(sender());

    if (buttonPressed == ui->elevator1Help || buttonPressed == ui->elevator2Help || buttonPressed == ui->elevator3Help)
        helpRequested = true;
    else if (buttonPressed == ui->fireButton)
        fireEmergency = true;
    else powerOutEmergency = true;
}
