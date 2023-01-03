#include "util.h"

std::random_device dev;
std::mt19937       rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist0to14(0, 14);

/*
    returns a random number from 0 to 14
    used from random numbers for passengers picking floor,
    random chance of passenger not entering elevator property and
    other random things
*/

int randomInteger0to14() {
    return dist0to14(rng);
}
