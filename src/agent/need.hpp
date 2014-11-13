#ifndef NEED_HPP
#define NEED_HPP
struct Need
{
    unsigned char currentValue;
    int needID;
    //If the currentValue is greater than fulfilledThreshold, the
    //agent will NEVER run a process chain to fulfill this need
    unsigned char fulfilledThreshold;
    //Mutated values
    //char decreaseMultiplier;
    //char increaseMultiplier;
};
#endif
