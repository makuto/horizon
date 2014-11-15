#ifndef SPECIES_HPP
#define SPECIES_HPP
#include <base2.0/ept/eptParser.hpp>
#include <vector>
#include <map>
#include "agent.hpp"
#include "needProcessor.hpp"
#include "../world/time.hpp"
#include "processDirectory.hpp"
/* --Species--
 * Species abstracts the processing of agents of that species
 * Agents contain only their own data, so species must provide
 * the logic.
 * Species are created via an ept file containing all of the needs
 * required by the agent. These populate the NeedProcessor vectors
 * that are then looped through in order to update the agent's needs*/

class Species
{
    private:
        //These exactly line up with vital needs in the agent
        //They are pointers to NeedProcessors in needProcessDir
        std::vector<NeedProcessor*> vitalNeedProcessors;
        std::vector<NeedProcessor*> nonvitalNeedProcessors;
        int numVitalNeeds;
        int numNonvitalNeeds;
    public:
        int speciesID;
        Species(eptFile* spec, std::map<std::string, NeedProcessor*>* needProcessorDir);
        ~Species();
        //updateAgent returns -1 if the agent should die
        //Pass in the difference in time since the last agent update
        //Note that deltaTime is only used for agent processes, not need updates
        int updateAgent(Agent* agent, Time* globalTime, Time* deltaTime, ProcessDirectory* processDir);
        Agent* createAgent();
};
#endif
