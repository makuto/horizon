#ifndef PROCESS_CPP
#define PROCESS_CPP
#include <iostream>
#include "process.hpp"
#include "agent.hpp"
#include "needMath.hpp"
Process::~Process()
{
    
}
int Process::getDifficulty(Agent* agent, Object* obj, Need* need, int index)
{
    return 1;
}
//Update should return 0 if process didn't complete (run next frame)
//1 if process is ready to go to the next process in chain
//-1 if process chain should end immediately
int Process::update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime)
{
    //std::cout << "Updated Process; index: "<< agent->currentProcessIndex <<"\n";
    if (agent->currentProcessIndex==0)
    {
        //agent->worldPosition.addVector(0, -60);
        //obj->addVector(0, -5, *objectManager);
        counter++;
        if (counter>=10)
        {
            need->currentValue=addNoOverflow(need->currentValue, 255);
            counter = 0;
            return 1;
        }
        //TODO: REPLACE NEEDPROCESSOR UPDATE WITH SIMPLE GENERIC DECAY?
        //TODO: Probably need to use floats or two chars due to very low update times (need.currentValue)
        return 0;
    }
    return 1;
}
#endif
