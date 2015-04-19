#ifndef RESOURCETREE_CPP
#define RESOURCETREE_CPP
#include "resourceTree.hpp"
#include "world.hpp"

const int RESOURCE_SMALL_RADIUS = 256;
const int RESOURCE_MEDIUM_RADIUS = 1024;
const int RESOURCE_LARGE_RADIUS =  2048;

const unsigned int RESOURCETREE_POOL_SIZE = 500;

ResourceTree::ResourceTree(World* newWorld):resourcePool(RESOURCETREE_POOL_SIZE)
{
    world = newWorld;
    //Initialize layer maps
    tree[NEED_BENEFITS] = new RESOURCE_CELL;
    tree[BUILDING_RESOURCES] = new RESOURCE_CELL;
}
ResourceTree::~ResourceTree()
{
    //Delete all of the lists and cell maps
    for(std::map<LAYER_TYPE, RESOURCE_CELL* >::iterator lIt = tree.begin(); lIt != tree.end(); ++lIt)
    {
        for (RESOURCE_CELL::iterator cIt = lIt->second->begin();
        cIt != lIt->second->end(); ++cIt)
        {
            delete cIt->second;
        }
        delete lIt->second;
    }
}
RESOURCE_LIST* ResourceTree::getResourceList(LAYER_TYPE layer, CellIndex& cell, bool createIfNone)
{
    RESOURCE_CELL* currentLayer = tree[layer];
    //Find the cell requested; if it doesn't exist, create it
    RESOURCE_LIST* resourceList = NULL;
    RESOURCE_CELL::iterator cIt = currentLayer->find(cell);
    if (cIt==currentLayer->end()) 
    {
        if (createIfNone)
        {
            resourceList = new RESOURCE_LIST;
            (*currentLayer)[cell] = resourceList;
        }
    }
    else resourceList = cIt->second;
    return resourceList;
}
void ResourceTree::addResource(LAYER_TYPE layer, Resource::TYPE type, int subType, Coord& position)
{
    CellIndex cell = position.getCell();
    RESOURCE_LIST* resourceList = getResourceList(layer, cell);

    //Get a new Resource from the pool
    PoolData<Resource>* newResource = resourcePool.getNewData();
    if (!newResource)
    {
        std::cout << "ERROR: ResourceTree.addResource(): Pool is full!\n";
        return;
    }
    newResource->data.type = type;
    newResource->data.subType = subType;
    newResource->data.position = position;
    resourceList->push_back(newResource);
}
void ResourceTree::removeResource(LAYER_TYPE layer, Resource::TYPE type, int subType, Coord& position)
{
    CellIndex cell = position.getCell();
    RESOURCE_LIST* resourceList = getResourceList(layer, cell);
    for (RESOURCE_LIST::iterator lIt = resourceList->begin(); lIt != resourceList->end(); ++lIt)
    {
        Resource* currentResource = &(*lIt)->data;
        if (currentResource->type == type && currentResource->subType == subType
            && currentResource->position == position)
        {
            resourcePool.removeData((*lIt));
            resourceList->erase(lIt);
            break;
        }
    }
}
//Searches current cell and nearby cells in radius for all resources
//on specified layer. Returns the number of results
int ResourceTree::findResources(LAYER_TYPE layer, Coord& position, int radius, Resource** resultsArray, int maxResults)
{
    //Get all cells within the requested radius
    Coord topLeft;
    CellIndex cell = position.getCell();
    topLeft.setPosition(cell, position.getCellOffsetX(), position.getCellOffsetY());
    topLeft.addVector(-radius, -radius);
    int width = radius * 2;
    int height = radius * 2;
    int resultsSize;
    CellIndex* cellsToCheck = world->getIntersectingCells(topLeft, width, height, resultsSize);
    int totalResources = 0;
    for (int i = 0; i < resultsSize; i++)
    {
        RESOURCE_LIST* currentCell = getResourceList(layer, cellsToCheck[i], false);
        if (currentCell)
        {
            for (RESOURCE_LIST::iterator lIt = currentCell->begin(); lIt != currentCell->end(); ++lIt)
            {
                resultsArray[totalResources] = &(*lIt)->data;
                totalResources++;
                if (totalResources >= maxResults) return totalResources - 1;
            }
        }
    }
    return totalResources;
}
//Searches current cell and nearby cells in radius for all resources
//on specified layer, then filters resources depending on provided TYPE.
//Returns the number of results
int ResourceTree::findResourcesOfType(LAYER_TYPE layer, Resource::TYPE type, Coord& position, int radius, Resource** resultsArray, int maxResults)
{
    //Get all cells within the requested radius
    Coord topLeft;
    CellIndex cell = position.getCell();
    topLeft.setPosition(cell, position.getCellOffsetX(), position.getCellOffsetY());
    topLeft.addVector(-radius, -radius);
    int width = radius * 2;
    int height = radius * 2;
    int resultsSize;
    CellIndex* cellsToCheck = world->getIntersectingCells(topLeft, width, height, resultsSize);
    int totalResources = 0;
    for (int i = 0; i < resultsSize; i++)
    {
        RESOURCE_LIST* currentCell = getResourceList(layer, cellsToCheck[i], false);
        if (currentCell)
        {
            for (RESOURCE_LIST::iterator lIt = currentCell->begin(); lIt != currentCell->end(); ++lIt)
            {
                Resource* currentResource = &(*lIt)->data;
                if (currentResource->type != type) continue; //Filter by type
                resultsArray[totalResources] = currentResource;
                totalResources++;
                if (totalResources >= maxResults) return totalResources - 1;
            }
        }
    }
    return totalResources;
    return 0;
}
//Searches current cell and nearby cells in radius for all resources
//on specified layer, then filters resources depending on provided subType.
//Returns the number of results
int ResourceTree::findResourcesOfSubType(LAYER_TYPE layer, int subType, Coord& position, int radius, Resource** resultsArray, int maxResults)
{
    //Get all cells within the requested radius
    Coord topLeft;
    CellIndex cell = position.getCell();
    topLeft.setPosition(cell, position.getCellOffsetX(), position.getCellOffsetY());
    topLeft.addVector(-radius, -radius);
    int width = radius * 2;
    int height = radius * 2;
    int resultsSize;
    CellIndex* cellsToCheck = world->getIntersectingCells(topLeft, width, height, resultsSize);
    int totalResources = 0;
    //std::cout << "Checking " << resultsSize << " cells\n";
    for (int i = 0; i < resultsSize; i++)
    {
        RESOURCE_LIST* currentCell = getResourceList(layer, cellsToCheck[i], false);
        if (currentCell)
        {
            for (RESOURCE_LIST::iterator lIt = currentCell->begin(); lIt != currentCell->end(); ++lIt)
            {
                Resource* currentResource = &(*lIt)->data;
                //std::cout << "  Resource type " << currentResource->type << " subType " << currentResource->subType << "\n";
                if (currentResource->subType != subType) continue; //Filter by subType
                resultsArray[totalResources] = currentResource;
                totalResources++;
                if (totalResources >= maxResults) return totalResources - 1;
            }
        }
    }
    return totalResources;
    return 0;
}
#endif
