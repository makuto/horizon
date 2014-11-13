#include <iostream>
#include "species.hpp"
int Species::updateAgent(Agent* agent)
{
    std::cout << agent->id;
    return 1;
}
Agent* Species::createAgent()
{
    Agent* newAgent = new Agent;
    newAgent->id = 12;
    newAgent->mutationSeed = 123213;
    newAgent->species = 1;
    newAgent->vitalNeeds = new Need[10];
    newAgent->nonvitalNeeds = new Need[2];
    return newAgent;
}
