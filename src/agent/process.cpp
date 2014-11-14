#ifndef PROCESS_CPP
#define PROCESS_CPP
#include <iostream>
#include "process.hpp"
#include "agent.hpp"
#include "needMath.hpp"
Process::~Process()
{
    
}
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
int Process::update(Agent* agent, Need* need, Time* currentTime)
{
    std::cout << "Updated Process; index: "<< agent->currentProcessIndex <<"\n";
    if (agent->currentProcessIndex==0)
    {
        need->currentValue=addNoOverflow(need->currentValue, 60);
        return 1;
    }
    return 1;
}
#endif
