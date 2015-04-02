#ifndef USEITEM_PROCESS_CPP
#define USEITEM_PROCESS_CPP
#include <iostream>
#include "useItemProcess.hpp"
#include "../agent.hpp"
#include "../needMath.hpp"

UseItemProcess::~UseItemProcess()
{
    
}
int UseItemProcess::getDifficulty(Agent* agent, Need* need)
{
    return 1;
}
unsigned char UseItemProcess::getValue(Agent* agent, Need* need)
{
    return 1;
}
//Update should return 0 if process didn't complete (run next frame)
//1 if process is ready to go to the next process in chain
//-1 if process chain should end immediately
int UseItemProcess::update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime)
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
