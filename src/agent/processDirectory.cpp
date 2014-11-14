#ifndef PROCESSDIRECTORY_CPP
#define PROCESSDIRECTORY_CPP
#include <iostream>
#include "processDirectory.hpp"
const int MAX_DIFFICULTY = 1000000;
ProcessDirectory::ProcessDirectory(eptParser* parser, eptFile* needFileListing, ProcessMap* processes)
{
    std::string groupName;
    eptGroup* currentGroup=needFileListing->getGroupFromIndex(0, groupName);
    for (int i = 1; currentGroup!=NULL; i++)
    {
        //Load chain file
        std::string chainFileName = currentGroup->getAttribute("chains");
        if (!parser->load(chainFileName.c_str()))
        {
            std::cout << "ERROR: Failed to load '" << chainFileName << "' chains file\n";
            return;
        }
        else //Parse chains file (for one need)
        {
            int needID  = attrToInt(currentGroup->getAttribute("id"));
            //Create a new vector for this needs' chains
            directory[needID] = new std::vector<ProcessChain*>;
            std::string chainsEPTName = currentGroup->getAttribute("id") + "_chains";
            eptFile* chainSpec = parser->getFile(chainsEPTName);
            if (!chainSpec)
            {
                std::cout << "ERROR: Failed to find chain file '" << chainsEPTName << "'. Did you name it correctly?\n";
                return;
            }
            std::string chainName;
            eptGroup* currentChain = chainSpec->getGroupFromIndex(0, chainName);
            for (int n = 1; currentChain!=NULL; n++) //Parse each chain
            {
                //New chain vector
                ProcessChain* newChain = new ProcessChain;
                //Get the processes for the current chain
                for (int r = 0; r < currentChain->getTotalAttributes(); r++)
                {
                    std::string processAttrName;
                    std::string processName = currentChain->getAttributeFromIndex(r, processAttrName);
                    Process* newProcess = processes->getProcess(processName);
                    if (!newProcess)
                    {
                        std::cout << "ERROR: failed to find process '" << processName << "'in processMap\n";
                        delete newChain;
                        return;
                    }
                    newChain->push_back(newProcess);
                }
                //Add the finished chain to the map
                directory[needID]->push_back(newChain);
                //Go to the next chain
                currentChain = chainSpec->getGroupFromIndex(n, chainName);
            }
            
        }
        //Move to next need
        currentGroup = needFileListing->getGroupFromIndex(i, groupName);
    }
}
ProcessDirectory::~ProcessDirectory()
{
    for (std::map<int, std::vector<ProcessChain*>* >::iterator it=directory.begin(); it!=directory.end(); it++)
    { //Loop through map
        for (std::vector<ProcessChain*>::iterator pIt = it->second->begin(); pIt != it->second->end(); pIt++)
        { //Loop through vector
            //Delete ProcessChain
            delete (*pIt);
        }
        //Delete vector
        delete it->second;
    }
}
std::vector<ProcessChain*>* ProcessDirectory::getNeedListings(int needID)
{
    std::map<int, std::vector<ProcessChain*>* >::iterator it=directory.find(needID);
	if (it==directory.end()) return NULL;
	else return it->second;
}
//TODO: use Process.getValue()?
ProcessChain* ProcessDirectory::getLeastDifficultyChain(Agent* agent, Need* need, std::vector<ProcessChain*>* listing, int& difficulty)
{
    int minimumDifficulty = MAX_DIFFICULTY;
    ProcessChain* currentBestChain = NULL;
    //Loop through all listings
    for (std::vector<ProcessChain*>::iterator it = listing->begin(); it !=listing->end(); it++)
    {
        int totalDifficulty = 0;
        //Loop through each process in the chain and get its difficulty
        for (ProcessChain::iterator pIt=(*it)->begin(); pIt!=(*it)->end(); pIt++)
        {
            int currentDifficulty = (*pIt)->getDifficulty(agent, need);
            if (currentDifficulty == -1) //Impossible process
            {
                totalDifficulty = MAX_DIFFICULTY;
                break;
            }
            totalDifficulty += currentDifficulty;
        }
        if (totalDifficulty < minimumDifficulty)
        {
            //Found new best chain (lowest difficulty)
            currentBestChain = (*it);
            minimumDifficulty = totalDifficulty;
        }
    }
    difficulty = minimumDifficulty;
    return currentBestChain;
}
ProcessChain* ProcessDirectory::getOptimalChain(Agent* agent, Need* need, int needID, int& difficulty)
{
    std::vector<ProcessChain*>* listings = getNeedListings(needID);
    if (!listings) return NULL;
    return getLeastDifficultyChain(agent, need, listings, difficulty);
}
#endif
