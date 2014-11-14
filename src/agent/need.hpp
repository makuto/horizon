#ifndef NEED_HPP
#define NEED_HPP
#include "../world/time.hpp"
struct Need
{
    unsigned char currentValue;
    int needID;
    //If the currentValue is greater than fulfilledThreshold, the
    //agent will NEVER run a process chain to fulfill this need
    unsigned char fulfilledThreshold;
    //The global time that this need was last updated
    Time lastUpdateGlobal;
    //How often to update (if current global time - lastUpdateGlobal
    //is greater than updateRate, run updateNeed)
    Time updateRate;
    //Mutated values
    //char decreaseMultiplier;
    //char increaseMultiplier;
};
#endif
