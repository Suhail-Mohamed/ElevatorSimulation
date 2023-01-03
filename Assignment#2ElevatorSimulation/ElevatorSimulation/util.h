#ifndef UTIL_H
#define UTIL_H

#include <random>

const int SAFE_FLOOR = 0;

enum Direction      { Up = 1, Down = -1, NotMoving = 0 };
enum SpecialButton  { Hold, Close, NoButtonClicked };
enum PassengerError { CoveringSensor, WeightOverload, NoError };


int randomInteger0to14();

#endif // UTIL_H
