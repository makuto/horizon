#ifndef AGENT_HPP
#define AGENT_HPP
#include "need.hpp"
#include "process.hpp"
#include "../world/coord.hpp"
struct Agent
{
    int id;
    int mutationSeed;
    int species;
    Need* vitalNeeds;
    Need* nonvitalNeeds;
    //TODO: Add a "careerNeeds" array for individual career needs
    ProcessChain* currentProcessChain;
    int currentProcessIndex;   //-1 if there is no current process or chain
    int processChainVitalNeedID;    //Tells us which need we are trying to fulfill
    int processChainNonvitalNeedID;    //-1 if Vital need is being fulfilled
    //Coord worldPosition;  //Agents are now going to be tied to Objects, so
                            //position is redundant
    
};

#endif
