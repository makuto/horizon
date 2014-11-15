#ifndef PROCESS_HPP
#define PROCESS_HPP
#include <vector>
#include "../world/time.hpp"
//#include "agent.hpp" //Forward declared
#include "need.hpp"

/* --Process--
 * A Process is an action an agent can do that moves them towards a goal.
 * These are designed to be overloaded.
 * For example, an agent has a simple "hunger" need that gets to detriment
 * threshold and becomes priority. Process trees are then searched for
 * the Process or combination of Processes that has the lowest difficulty
 * for the most supplement to the need. For "hunger," you might have a
 * Process that runs if the agent has a Food item or something. You also
 * have a chain of Processes that searches for Food, pathfinds to the Food,
 * then eats the Food.
 * Let's say the agent doesn't have any Food items -
 * that Process checks that in getDifficulty() and returns a difficulty of
 * -1, or impossible. The process chain is looped over and found to have
 * a difficulty of 986, which combines each individual process getDifficulty.
 * This chain is then selected because it has the lower difficulty (-1 being
 * the same as infinite difficulty). The agent then stores this process
 * chain as its current process chain, and marks the current process in
 * the chain.
 * */
struct Agent;
class Process;
//TODO: Should ProcessChain be its own class?
typedef std::vector<Process*> ProcessChain;

class Process
{
    public:
        virtual ~Process();
        //TODO: Is need needed? (UPDATE: Yes, so generic search processes
        //etc. know what to look for)
        virtual int getDifficulty(Agent* agent, Need* need);
        virtual unsigned char getValue(Agent* agent, Need* need);
        //Update should return 0 if process didn't complete (run next frame)
        //1 if process is ready to go to the next process in chain
        //-1 if process chain should end immediately
        virtual int update(Agent* agent, Need* need, Time* deltaTime);
};
#endif
