#ifndef PATHMANAGER_HPP
#define PATHMANAGER_HPP

#include <map>
#include "../utilities/pool.hpp"
#include "path.hpp"

class World;
class PathManager
{
    private:
        World* world;
        Pool<Path> paths;
        std::map<unsigned int, PoolData<Path>* > activePaths;
        unsigned int nextID;

        PoolData<Path>* getPath(unsigned int pathID);
    public:
        PathManager(World* newWorld);
        ~PathManager();
        
        //Pass in the start and end position. An ID will be returned for
        //the request. Returns 0 if the pool is full.
        unsigned int requestNewPath(Coord& startPosition, Coord& endPosition);
        //If you are finished with a path (or calculation fails), call this
        //function to free up the pool resources. Note that this will auto-
        //matically be called when the path says you are finished following
        //(this check happens in update())
        void freePath(unsigned int pathID);
        
        //Sets the passed Coord if the path is ready. Simply put this function
        //in a switch statement in your update function to handle returns
        //(getPathStatus is not required). If return is 1, do movement calculations
        //Returns 1 if ready, 0 if requiring calculation, -2 if the path
        //does not exist (in the pool), or -1 if failed
        int advanceAlongPath(unsigned int pathID, Coord &currentPosition, Coord& targetPosition);
        
        //Returns path status, or -2 if path does not exist in pool
        int getPathStatus(unsigned int pathID);
        //Returns estimated difficulty, or -2 if the path does not exist in the pool
        float getEstimatedDifficulty(unsigned int pathID);
        
        //Updates active paths that need calculation for <=timeToUse
        //Paths with failed statuses or finished statuses will be freed
        void update(float timeToUse);
};
#endif
