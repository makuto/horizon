#ifndef $T_PROCESS_CPP
#define $T_PROCESS_CPP
#include <iostream>
#include "CHANGEME.hpp"
#include "../agent.hpp"
#include "../needMath.hpp"

$tProcess::~$tProcess()
{
    
}
int $tProcess::getDifficulty(Agent* agent, Need* need, int index)
{
    return 1;
}
//Update should return 0 if process didn't complete (run next frame)
//1 if process is ready to go to the next process in chain
//-1 if process chain should end immediately
int $tProcess::update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime)
{
    if (agent->currentProcessIndex==0)
    {
        obj->addVector(0, -5, *objectManager);
        counter++;
        if (counter>=10)
        {
            need->currentValue=addNoOverflow(need->currentValue, 255);
            counter = 0;
            return 1;
        }
        return 0;
    }
    return 1;
}
#endif
