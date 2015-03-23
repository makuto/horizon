#ifndef PATHMANAGER_CPP
#define PATHMANAGER_CPP
#include "pathManager.hpp"
#include "../utilities/debugText.hpp"
#include <base2.0/timer/timer.hpp>

//The number of paths that can be active at once.
const unsigned int PATH_POOL_SIZE = 10;

PathManager::PathManager(World* newWorld):paths(PATH_POOL_SIZE)
{
    world = newWorld;
    nextID = 1;
}
PathManager::~PathManager()
{
    
}
PoolData<Path>* PathManager::getPath(unsigned int pathID)
{
    std::map<unsigned int, PoolData<Path>* >::iterator findIt = activePaths.find(pathID);
    if (findIt != activePaths.end()) return findIt->second;
    else return NULL;
}
//Pass in the start and end position. An ID will be returned for
//the request. Returns 0 if the pool is full.
unsigned int PathManager::requestNewPath(Coord& startPosition, Coord& endPosition)
{
    PoolData<Path>* newPath = paths.getNewData();
    if (!newPath) return 0;
    newPath->data.init(world, startPosition, endPosition);
    activePaths[nextID] = newPath;
    nextID++;
    return nextID - 1;
}
//Sets the passed Coord if the path is ready
//Returns 1 if ready, 0 if requiring calculation, -2 if the path
//does not exist (in the pool), or -1 if failed
int PathManager::advanceAlongPath(unsigned int pathID, Coord &currentPosition, Coord& targetPosition)
{
    PoolData<Path>* currentPathData = getPath(pathID);
    if (currentPathData)
    {
        Path* currentPath = &currentPathData->data;
        int status = currentPath->getStatus();
        if (status != 1) return status; //Path failed or not ready
        targetPosition = currentPath->advance(currentPosition);
        return 1;
        
    }
    else return -2; //Path not found
}
//Returns path status, or -2 if path does not exist in pool
int PathManager::getPathStatus(unsigned int pathID)
{
    PoolData<Path>* currentPathData = getPath(pathID);
    if (currentPathData)
    {
        return currentPathData->data.getStatus();
    }
    else return -2;
}
//Returns estimated difficulty, or -2 if the path does not exist in the pool
float PathManager::getEstimatedDifficulty(unsigned int pathID)
{
    PoolData<Path>* currentPathData = getPath(pathID);
    if (currentPathData)
    {
        return currentPathData->data.getEstimatedDifficulty();
    }
    else return -2;
}
//If you are finished with a path (or calculation fails), call this
//function to free up the pool resources. Note that this will auto-
//matically be called when the path says you are finished following
void PathManager::freePath(unsigned int pathID)
{
    std::map<unsigned int, PoolData<Path>* >::iterator findIt = activePaths.find(pathID);
    if (findIt != activePaths.end())
    {
        paths.removeData(findIt->second);
        activePaths.erase(findIt);
    }
}
//Updates active paths that need calculation for <=timeToUse
void PathManager::update(float timeToUse)
{
    timer currentTime;
    currentTime.start();

    for (std::map<unsigned int, PoolData<Path>* >::iterator it = activePaths.begin();
        it != activePaths.end(); ++it)
    {
        //Only calculate paths that need it (obviously)
        switch (it->second->data.getStatus())
        {
            case 0: //Path needs calculation
                it->second->data.calculate();
                break;
            case 2: //Path follower finished - free it
                freePath(it->first);
                break;
            case -1: //Path failed to be calculated - free it
                freePath(it->first);
                break;
            default: //Either 1 (path ready) or unknown status
                break;
        }
        
        //Stop calculating if we have used all the time
        if (currentTime.getTime() >= timeToUse) break;
    }
    //Debug
    DebugText::addEntry("Path Pool Usage (%): ", ((float)paths.getTotalActiveData() / (float)PATH_POOL_SIZE) * 100);
}
#endif

