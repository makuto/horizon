#ifndef $T_NEEDPROCESSOR_CPP
#define $T_NEEDPROCESSOR_CPP
#include <iostream>
#include "CHANGEME.hpp"
#include "../needMath.hpp"

 
$tNeedProcessor::$tNeedProcessor()
{
    
}
$tNeedProcessor::~$tNeedProcessor()
{
}
//Use this function to set defaults for this need (you must call this!)
bool $tNeedProcessor::initialize(eptFile* spec)
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
int $tNeedProcessor::updateNeed(Agent* agent, Object* obj, ObjectManager* objectManager, Need* currentNeed, Time* deltaTime)
{
    currentNeed->currentValue = subNoOverflow(currentNeed->currentValue, 50);
    return 1;
}
void $tNeedProcessor::initNeed(Need* currentNeed)
{
    currentNeed->currentValue = defaultNeed.currentValue;
    currentNeed->needID = defaultNeed.needID;
    currentNeed->fulfilledThreshold = defaultNeed.fulfilledThreshold;
    currentNeed->updateRate = defaultNeed.updateRate;
}

#endif
