#ifndef AGENT_HPP
#define AGENT_HPP
#include "need.hpp"
struct Agent
{
    int id;
    int mutationSeed;
    int species;
    Need* vitalNeeds;
    Need* nonvitalNeeds;
    
};

#endif
