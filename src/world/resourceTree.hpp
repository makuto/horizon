#ifndef RESOURCETREE_HPP
#define RESOURCETREE_HPP
#include <map>
#include <list>
#include "../utilities/pool.hpp"
#include "coord.hpp"
/* --ResourceTree--
 * ResourceTree stores the locations of notable Resources in the world;
 * for example, it will store the locations of food. Agents can then search
 * the ResourceTree for Resources that replenish their needs.
 * */

//Use these constants for radius, if you want
extern const int RESOURCE_SMALL_RADIUS;
extern const int RESOURCE_MEDIUM_RADIUS;
extern const int RESOURCE_LARGE_RADIUS;

struct Resource
{
    enum TYPE
    {
        OBJECT,
        TILE,
        ITEM
    };
    TYPE type;
    int subType;
    Coord position;
};

typedef std::list<PoolData<Resource>* > RESOURCE_LIST;
typedef std::map<CellIndex, RESOURCE_LIST*, CellIndexComparer> RESOURCE_CELL;

class World;
class ResourceTree
{
    public:
        enum LAYER_TYPE
        {
            NEED_BENEFITS,   //Resource that benefits a need
            BUILDING_RESOURCES  //Tile-based resources, e.g. quarries
        };
    private:
        Pool<Resource> resourcePool;
        std::map<LAYER_TYPE, RESOURCE_CELL*> tree;
        //Returns the RESOURCE_LIST for the requested cell, or creates a new
        //RESOURCE_LIST for that cell if it doesn't exist. getResourceList()
        //always returns a list, so you do not need to check if NULL (unless
        //createIfNone is false)
        RESOURCE_LIST* getResourceList(LAYER_TYPE layer, CellIndex& cell, bool createIfNone=true);

        World* world;
    public:
        ResourceTree(World* newWorld);
        ~ResourceTree();
        void addResource(LAYER_TYPE layer, Resource::TYPE type, int subType, Coord& position);
        void removeResource(LAYER_TYPE layer, Resource::TYPE type, int subType, Coord& position);
        //Searches current cell and nearby cells in radius for all resources
        //on specified layer. Returns the number of results
        int findResources(LAYER_TYPE layer, Coord& position, int radius, Resource** resultsArray, int maxResults);
        //Searches current cell and nearby cells in radius for all resources
        //on specified layer, then filters resources depending on provided TYPE.
        //Returns the number of results
        int findResourcesOfType(LAYER_TYPE layer, Resource::TYPE type, Coord& position, int radius, Resource** resultsArray, int maxResults);
        //Searches current cell and nearby cells in radius for all resources
        //on specified layer, then filters resources depending on provided subType.
        //Returns the number of results
        int findResourcesOfSubType(LAYER_TYPE layer, int subType, Coord& position, int radius, Resource** resultsArray, int maxResults);
};
#endif
