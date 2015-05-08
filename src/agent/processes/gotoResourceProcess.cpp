#ifndef GOTORESOURCE_PROCESS_CPP
#define GOTORESOURCE_PROCESS_CPP
#include <iostream>
#include "gotoResourceProcess.hpp"
#include "../agent.hpp"
#include "../needMath.hpp"
#include "../../world/resourceTree.hpp"
#include "../../world/pathManager.hpp"

const unsigned int RESOURCE_SEARCH_CACHE_SIZE = 10;

GoToResourceProcess::GoToResourceProcess()
{
    resourceSearchCache = new Resource*[RESOURCE_SEARCH_CACHE_SIZE];
}
GoToResourceProcess::~GoToResourceProcess()
{
    delete[] resourceSearchCache;
}
void GoToResourceProcess::setup(PathManager* newPathManager, ResourceTree* newResourceTree)
{
    pathManager = newPathManager;
    resourceTree = newResourceTree;
}
int GoToResourceProcess::getDifficulty(Agent* agent, Object* obj, Need* need, int index)
{
    //std::cout << "goToResource.getDifficulty()\n";
    Coord position = obj->getPosition();
    int numResults = resourceTree->findResourcesOfSubType(ResourceTree::LAYER_TYPE::NEED_BENEFITS,
        need->needID, position, RESOURCE_MEDIUM_RADIUS, resourceSearchCache, RESOURCE_SEARCH_CACHE_SIZE);
    //std::cout << "  NumResults: " << numResults << "\n";
    if (numResults == 0) return -1; //Impossible; no resources found
    for (int i = 0; i < numResults; i++)
    {
        //std::cout << "  " << resourceSearchCache[i]->type << " resource\n";
        //std::cout << "      Difficulty: " << (int)position.getManhattanTo(resourceSearchCache[i]->position) << "\n";
        //Return the manhattan distance to the resource as the difficulty
        return position.getManhattanTo(resourceSearchCache[i]->position); 
    }
    return -1;
}
//Update should return 0 if process didn't complete (run next frame)
//1 if process is ready to go to the next process in chain
//-1 if process chain should end immediately
int GoToResourceProcess::update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime)
{
    if (agent->targetID == -1)
    {
        Coord position = obj->getPosition();
        int numResults = resourceTree->findResourcesOfSubType(ResourceTree::LAYER_TYPE::NEED_BENEFITS,
            need->needID, position, RESOURCE_MEDIUM_RADIUS, resourceSearchCache, RESOURCE_SEARCH_CACHE_SIZE);
        if (numResults == 0) return -1; //Impossible; no resources found
        for (int i = 0; i < numResults; i++)
        {
            //Get a path to the first resource that will benefit this need
            agent->targetID = pathManager->requestNewPath(position, resourceSearchCache[i]->position);
            return 0;
        }
    }
    //Advance along path to resource
    Coord target;
    Coord objPos = obj->getPosition();
    switch(pathManager->advanceAlongPath((unsigned int)agent->targetID, objPos, target))
    {
        case 0: //Waiting for calculations
            return 0;
        case 1: //Ready
            obj->moveTowards(target, 300, deltaTime, *objectManager);
            return 0;
        case -1: //Path failed
            std::cout << "GoToResourceProcess: Path failed to calculate!\n";
            return -1;
        case -2: //Path doesn't exist - it's probably done
            return 1;
        default: //Unknown return
            return -1;
    }
    return 1;
}
#endif
