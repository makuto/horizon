#ifndef NEEDPROCESSOR_CPP
#define NEEDPROCESSOR_CPP
#include <iostream>
#include "needProcessor.hpp"
#include "needMath.hpp"
NeedProcessor::NeedProcessor(eptFile* spec)
{
    eptGroup* defaultNeedVars = spec->getGroup("defaults");
    defaultNeed.currentValue = (unsigned char)attrToInt(defaultNeedVars->getAttribute("currentValue"));
    defaultNeed.needID = attrToInt(defaultNeedVars->getAttribute("needID"));
    defaultNeed.fulfilledThreshold = (unsigned char)attrToInt(defaultNeedVars->getAttribute("fulfilledThreshold"));
}
NeedProcessor::~NeedProcessor()
{
}
int NeedProcessor::updateNeed(Agent* agent, Need* currentNeed, Time* deltaTime)
{
    std::cout << (int) currentNeed->currentValue << " (updateNeed)\n";
    std::cout << "dTime: " << deltaTime->seconds << " seconds, " << deltaTime->milliseconds << " ms\n";
    currentNeed->currentValue = subNoOverflow(currentNeed->currentValue, 10 * (deltaTime->seconds + deltaTime->milliseconds));
    agent->worldPosition.addVector(0, 10);
    return 1;
}
void NeedProcessor::initNeed(Need* currentNeed)
{
    currentNeed->currentValue = defaultNeed.currentValue;
    currentNeed->needID = defaultNeed.needID;
    currentNeed->fulfilledThreshold = defaultNeed.fulfilledThreshold;
}

#endif
