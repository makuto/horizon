#ifndef SPECIES_HPP
#define SPECIES_HPP
#include <base2.0/ept/eptParser.hpp>
#include <vector>
#include <map>
#include "../utilities/pool.hpp"
#include "agent.hpp"
#include "needProcessor.hpp"
#include "../world/time.hpp"
#include "processDirectory.hpp"

#include "../object/objectManager.hpp"
#include "../object/object.hpp"
/* --Species--
 * Species abstracts the processing of agents of that species
 * Agents contain only their own data, so species must provide
 * the logic.
 * Species are created via an ept file containing all of the needs
 * required by the agent. These populate the NeedProcessor vectors
 * that are then looped through in order to update the agent's needs
 *
 * Species contains a pool of all created agents of its species. Updating is on
 * an individual basis due to the complexity of the agent-object setup.
 *
 * Note that Species isn't completely new-less because Need arrays are still
 * allocated when createAgent is called.
 *
 * Additionally, note that ALL species agents MUST have at least one vital and
 * one nonvital need.*/

class Species
{
    private:
        Pool<Agent> agentPool;  //Pool of all agents
        std::map<unsigned int, PoolData<Agent>* > agentMap; //Map of agent pointers to their IDs
        //These exactly line up with vital needs in the agent
        //They are pointers to NeedProcessors in needProcessDir
        std::vector<NeedProcessor*> vitalNeedProcessors;
        std::vector<NeedProcessor*> nonvitalNeedProcessors;
        int numVitalNeeds;
        int numNonvitalNeeds;
        unsigned int nextNewID;
    public:
        int speciesID;
        //Initialize the Species with the provided spec. Set newNextID to
        //a value such that if you are loading a world, new agent IDs will
        //not conflict with loaded agent IDs
        Species(eptFile* spec, NeedProcessorDir* needProcessorDir, unsigned int newNextID);
        ~Species();
        //updateAgent returns -1 if the agent should die
        //Pass in the difference in time since the last agent update
        //Note that deltaTime is only used for agent processes, not need updates
        int updateAgent(Agent* agent, Object* obj, ObjectManager* objectManager, Time* globalTime, Time* deltaTime, ProcessDirectory* processDir);
        //TODO: Should this just init an object instead (easier pooling?)
        //Grabs an agent from the pool and initializes it
        //Pass in 0 if you want Species to set an ID (you might not
        //want to do this if you are creating a previously existing
        //agent; then, you want to set it to its previous ID)
        Agent* createAgent(unsigned int id);
        //Returns NULL if the agent does not exist
        Agent* getAgent(unsigned int id);
        //Remove an agent from the map and pool
        void removeAgent(unsigned int id);
};
#endif
