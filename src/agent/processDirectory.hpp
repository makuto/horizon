#ifndef PROCESSDIRECTORY_HPP
#define PROCESSDIRECTORY_HPP
#include <map>
#include <vector>
#include <base2.0/ept/eptParser.hpp>
#include "process.hpp"
#include "processMap.hpp"
#include "agent.hpp"
/* --ProcessDirectory--
 * ProcessDirectory creates and stores process chains that agents browse
 * in order to decide what to do. 
 * */
class ProcessDirectory
{
    private:
        //TODO: Should this be a vector or map?
        std::map<int, std::vector<ProcessChain*>* > directory;

        std::vector<ProcessChain*>* getNeedListings(int needID);
        ProcessChain* getLeastDifficultyChain(Agent* agent, Need* need, std::vector<ProcessChain*>*, int& difficulty);
    public:
        //Parses the directory specified for each need, parsing chain files and linking
        //chains from the chain files using ProcessMap Processes.
        //needFileListing should be a pointer to an eptFile containing
        //groups with the need id, spec, and chains. The latter two should
        //be paths to ept files with their respective information
        ProcessDirectory(eptParser* parser, eptFile* needFileListing, ProcessMap* processes);
        ~ProcessDirectory();
        //Finds chains that fulfill the specified needID and calculates
        //which chain is the most optimal
        ProcessChain* getOptimalChain(Agent* agent, Need* need, int needID, int& difficulty);
};
#endif
