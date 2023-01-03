#include "ecs.h"
#include <iostream>

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication          a(argc, argv);
    ElevatorControlSystem ecs;

    ecs.show();

    return a.exec();
}
