#include "agent.hpp"
/* --Species--
 * Species abstracts the processing of agents of that species
 * Agents contain only their own data, so species must provide
 * the logic. */

class Species
{
    public:
        int speciesID;
        virtual int updateAgent(Agent* agent);
        virtual Agent* createAgent();
};
