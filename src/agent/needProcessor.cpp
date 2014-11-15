#ifndef NEEDPROCESSOR_CPP
#define NEEDPROCESSOR_CPP
#include <iostream>
#include "needProcessor.hpp"
#include "needMath.hpp"
NeedProcessor::NeedProcessor(eptFile* spec)
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
}
NeedProcessor::~NeedProcessor()
{
}
int NeedProcessor::updateNeed(Agent* agent, Need* currentNeed, Time* deltaTime)
{
    std::cout << (int) currentNeed->currentValue << " (updateNeed)\n";
    //currentNeed->currentValue = subNoOverflow(currentNeed->currentValue, 10 * (deltaTime->seconds + deltaTime->milliseconds));
    currentNeed->currentValue = subNoOverflow(currentNeed->currentValue, 10);
    agent->worldPosition.addVector(0, 10);
    return 1;
}
void NeedProcessor::initNeed(Need* currentNeed)
{
    currentNeed->currentValue = defaultNeed.currentValue;
    currentNeed->needID = defaultNeed.needID;
    currentNeed->fulfilledThreshold = defaultNeed.fulfilledThreshold;
    currentNeed->updateRate = defaultNeed.updateRate;
}

#endif
