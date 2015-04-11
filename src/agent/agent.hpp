#ifndef AGENT_HPP
#define AGENT_HPP
#include "need.hpp"
#include "process.hpp"
#include "../world/coord.hpp"
#include "../item/item.hpp"

extern const int AGENT_INVENTORY_SIZE;

struct Agent
{
    int id;
    int mutationSeed;
    int species;
    Item inventory[10]; //MAKE SURE AGENT_INVENTORY_SIZE MATCHES THIS
    Need* vitalNeeds;
    Need* nonvitalNeeds;
    int numVitalNeeds;
    int numNonvitalNeeds;
    //TODO: Add a "careerNeeds" array for individual career needs
    ProcessChain* currentProcessChain;
    int currentProcessIndex;   //-1 if there is no current process or chain
    int processChainVitalNeedID;    //Tells us which need we are trying to fulfill
    int processChainNonvitalNeedID;    //-1 if Vital need is being fulfilled
    //Coord worldPosition;  //Agents are now going to be tied to Objects, so
                            //position is redundant
    int targetID;   //Used by Processes for whatever is needed; for example,
                    //this could store a Path ID or even inventory index 
};

#endif
