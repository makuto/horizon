#ifndef AGENT_HPP
#define AGENT_HPP
#include "need.hpp"
#include "process.hpp"
struct Agent
{
    int id;
    int mutationSeed;
    int species;
    Need* vitalNeeds;
    Need* nonvitalNeeds;
    ProcessChain* currentProcessChain;
    int currentProcessIndex; //-1 if there is no current process or chain
    int processChainVitalNeedID; //Tells us which need we are trying to fulfill
    int processChainNonvitalNeedID; //-1 if Vital need is being fulfilled
    
};

#endif
