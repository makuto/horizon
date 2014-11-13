#ifndef PROCESS_CPP
#define PROCESS_CPP
#include <iostream>
#include "process.hpp"
int Process::getDifficulty(Agent* agent, Need* need)
{
    return 1;
}
unsigned char Process::getValue(Agent* agent, Need* need)
{
    return 1;
}
//Update should return 0 if process didn't complete (run next frame)
//1 if process is ready to go to the next process in chain
//-1 if process chain should end immediately
int Process::update(Agent* agent, Time* currentTime)
{
    std::cout << "Updated Process\n";
    return 1;
}
#endif
