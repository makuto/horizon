#ifndef SPECIES_CPP
#define SPECIES_CPP
#include <iostream>
#include "species.hpp"

//TODO: Replace needProcessorDir with a class?
Species::Species(eptFile* spec, std::map<std::string, NeedProcessor*>* needProcessorDir)
{
    eptGroup* vitalNeedsGroup = spec->getGroup("vitalNeeds");
    eptGroup* nonvitalNeedsGroup = spec->getGroup("nonvitalNeeds");
    
    //Set member variables to the spec's required needs
    numVitalNeeds = vitalNeedsGroup->getTotalAttributes();
    numNonvitalNeeds = nonvitalNeedsGroup->getTotalAttributes();
    //Get the species ID
    speciesID = attrToInt(spec->getGroup("vars")->getAttribute("id"));
    
    //Get the vital need processors as specified in the spec
    for (int i = 0; i < numVitalNeeds; i++)
    {
        std::string attrName;
        vitalNeedProcessors.push_back((*needProcessorDir)[vitalNeedsGroup->getAttributeFromIndex(i, attrName)]);
    }
    //Get the nonvital need processors as specified in the spec
    for (int i = 0; i < numNonvitalNeeds; i++)
    {
        std::string attrName;
        nonvitalNeedProcessors.push_back((*needProcessorDir)[nonvitalNeedsGroup->getAttributeFromIndex(i, attrName)]);
    }
}
Species::~Species()
{
    /*//Get rid of all unneccessary needProcessors
    for (std::vector<NeedProcessor*>::iterator it = vitalNeedProcessors.begin();
        it != vitalNeedProcessors.end(); ++it)
    {
        delete (*it);
    }
    for (std::vector<NeedProcessor*>::iterator it = nonvitalNeedProcessors.begin();
        it != nonvitalNeedProcessors.end(); ++it)
    {
        delete (*it);
    }*/ //NeedProcessors are taken from a directory, so they don't need deletion here
}
int Species::updateAgent(Agent* agent, Time* globalTime, Time* deltaTime, ProcessDirectory* processDir)
{
    std::cout << "---------------Updating agent " << agent->id << "\n";
    //Find the lowest need and process that one
    unsigned char minVitalValue = 255;
    unsigned char minNonvitalValue = 255;
    int minVitalIndex = -1;
    int minNonvitalIndex = -1;

    //Update vital needs; watch for lowest need currentValue
    for (int i = 0; i < numVitalNeeds; ++i)
    {
        //Get time since last update
        Time delta;
        agent->vitalNeeds[i].lastUpdateGlobal.getDeltaTime(globalTime, delta);
        //delta.print();
        if (delta.isGreaterThan(&agent->vitalNeeds[i].updateRate))
        {
            //Update need
            vitalNeedProcessors[i]->updateNeed(agent, &agent->vitalNeeds[i], deltaTime);
            agent->vitalNeeds[i].lastUpdateGlobal = *globalTime;
        }
        //Check if need is the lowest
        int currentNeedValue = agent->vitalNeeds[i].currentValue;
        if (currentNeedValue < minVitalValue)
        {
            minVitalValue = currentNeedValue;
            minVitalIndex = i;
        }
        
    }
    //Update nonvital needs; watch for lowest need currentValue
    for (int i = 0; i < numNonvitalNeeds; ++i)
    {
        //Get time since last update
        Time delta;
        agent->nonvitalNeeds[i].lastUpdateGlobal.getDeltaTime(globalTime, delta);
        //delta.print();
        if (delta.isGreaterThan(&agent->nonvitalNeeds[i].updateRate))
        {
            //Update need
            nonvitalNeedProcessors[i]->updateNeed(agent, &agent->nonvitalNeeds[i], deltaTime);
            agent->nonvitalNeeds[i].lastUpdateGlobal = *globalTime;
        }
        //Check if need is the lowest
        int currentNeedValue = agent->nonvitalNeeds[i].currentValue;
        if (currentNeedValue < minNonvitalValue)
        {
            minNonvitalValue = currentNeedValue;
            minNonvitalIndex = i;
        }
    }
    //Look for a process chain if the minimum need is below fulfilledThreshold
    if (minVitalValue < agent->vitalNeeds[minVitalIndex].fulfilledThreshold)
    {
        //The currently processed vital need value is greater than the
        //new lowest vital, so process the new one instead (or there is no
        //vital need currently being processed)
        if (agent->processChainVitalNeedID != minVitalIndex ||
        agent->processChainVitalNeedID==-1)
        {
            int needID = agent->vitalNeeds[minVitalIndex].needID;
            int difficulty;
            ProcessChain* optimalChain = processDir->getOptimalChain(agent, &agent->vitalNeeds[minVitalIndex], needID, difficulty);
            if (optimalChain)
            {
                agent->currentProcessChain = optimalChain;
                agent->currentProcessIndex = 0;
                agent->processChainVitalNeedID = minVitalIndex;
                agent->processChainNonvitalNeedID = -1;
            }
        }
        //Otherwise, continue on the current process (or give time for
        //nonvital processes)
    }
    else if (minNonvitalValue < agent->nonvitalNeeds[minNonvitalIndex].fulfilledThreshold)
    {
        //Instead of stopping the process if the minimum need is lower than
        //the currently processing need (like above for vital needs),
        //we will allow it to continue because it won't cause death, and
        //to prevent nonvital thrashing
        if (agent->processChainNonvitalNeedID==-1)
        {
            int needID = agent->nonvitalNeeds[minNonvitalIndex].needID;
            int difficulty;
            ProcessChain* optimalChain = processDir->getOptimalChain(agent, &agent->nonvitalNeeds[minNonvitalIndex], needID, difficulty);
            if (optimalChain)
            {
                agent->currentProcessChain = optimalChain;
                agent->currentProcessIndex = 0;
                agent->processChainVitalNeedID = -1;
                agent->processChainNonvitalNeedID = minNonvitalIndex;
            }
        }
        //Else continue with the current process
    }
    //Update the current process chain
    if (agent->currentProcessIndex!=-1)
    {
        //TODO: Replace NULL with need
        Need* needToProcess = NULL;
        if (agent->processChainNonvitalNeedID != -1) //currently processing nonvital
        {
            needToProcess = &agent->nonvitalNeeds[agent->processChainNonvitalNeedID];
            //std::cout << "Processing NONvital need; val: " << (int)needToProcess->currentValue << " threshold: " << (int)needToProcess->fulfilledThreshold << "\n";
        }
        if (agent->processChainVitalNeedID != -1) //currently processing vital
        {
            needToProcess = &agent->vitalNeeds[agent->processChainVitalNeedID];
            //std::cout << "Processing vital need; val: " << (int)needToProcess->currentValue << " threshold: " << (int)needToProcess->fulfilledThreshold << "\n";
        }
        int result = (*agent->currentProcessChain)[agent->currentProcessIndex]->update(agent, needToProcess, deltaTime);
        if (result==1) agent->currentProcessIndex++; //Move to next process in chain
        if (result==-1 || (unsigned int) agent->currentProcessIndex >= agent->currentProcessChain->size()) //Process chain finished
        {
            agent->currentProcessIndex=-1;
            agent->processChainNonvitalNeedID=-1;
            agent->processChainVitalNeedID=-1;
            agent->currentProcessChain=NULL;
        }
    }
    //Else idle (no needs need processing)
    
    return 1;
}
Agent* Species::createAgent()
{
    Agent* newAgent = new Agent;
    //TODO
    newAgent->id = 12;
    newAgent->mutationSeed = 123213;
    newAgent->species = speciesID;
    //Set defaults for processes
    newAgent->processChainNonvitalNeedID = -1;
    newAgent->processChainVitalNeedID = -1;
    newAgent->currentProcessChain = NULL;
    newAgent->currentProcessIndex = -1;
    
    //TODO: Allocate these in a pool of some sort
    newAgent->vitalNeeds = new Need[numVitalNeeds];
    newAgent->nonvitalNeeds = new Need[numNonvitalNeeds];
    
    //Set defaults for vital needs
    for (int i = 0; i < numVitalNeeds; ++i)
    {
        vitalNeedProcessors[i]->initNeed(&newAgent->vitalNeeds[i]);
    }
    //Set defaults for nonvital needs
    for (int i = 0; i < numVitalNeeds; ++i)
    {
        nonvitalNeedProcessors[i]->initNeed(&newAgent->nonvitalNeeds[i]);
    }
    return newAgent;
}
#endif
