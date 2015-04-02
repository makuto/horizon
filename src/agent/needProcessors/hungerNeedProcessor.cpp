#ifndef HUNGER_NEEDPROCESSOR_CPP
#define HUNGER_NEEDPROCESSOR_CPP
#include <iostream>
#include "hungerNeedProcessor.hpp"
#include "../needMath.hpp"
 
HungerNeedProcessor::HungerNeedProcessor()
{
}
HungerNeedProcessor::~HungerNeedProcessor()
{
}
bool HungerNeedProcessor::initialize(eptFile* spec)
{
    eptGroup* defaultNeedVars = spec->getGroup("defaults");
    defaultNeed.currentValue = (unsigned char)attrToInt(defaultNeedVars->getAttribute("currentValue"));
    needID = attrToInt(defaultNeedVars->getAttribute("needID"));
    defaultNeed.needID = needID;
    defaultNeed.fulfilledThreshold = (unsigned char)attrToInt(defaultNeedVars->getAttribute("fulfilledThreshold"));
    defaultNeed.updateRate.setMilliseconds(attrToFloat(defaultNeedVars->getAttribute("updateRateMS")));
    defaultNeed.updateRate.setSeconds(attrToInt(defaultNeedVars->getAttribute("updateRateSeconds")));
    defaultNeed.updateRate.setDays(attrToInt(defaultNeedVars->getAttribute("updateRateDays")));
    defaultNeed.updateRate.setYears(attrToInt(defaultNeedVars->getAttribute("updateRateYears")));
    return true;
}
int HungerNeedProcessor::updateNeed(Agent* agent, Object* obj, ObjectManager* objectManager, Need* currentNeed, Time* deltaTime)
{
    currentNeed->currentValue = subNoOverflow(currentNeed->currentValue, 10);
    return 1;
}
void HungerNeedProcessor::initNeed(Need* currentNeed)
{
    currentNeed->currentValue = defaultNeed.currentValue;
    currentNeed->needID = defaultNeed.needID;
    currentNeed->fulfilledThreshold = defaultNeed.fulfilledThreshold;
    currentNeed->updateRate = defaultNeed.updateRate;
}

#endif
