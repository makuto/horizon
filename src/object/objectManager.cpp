#ifndef OBJECTMANAGER_CPP
#define OBJECTMANAGER_CPP
#include "objectManager.hpp"
#include <vector>
#include <iostream>
#include <base2.0/collision/collision.hpp>
#include "../world/cell.hpp"
const unsigned int MAX_NODE_CAPACITY = 4;
const unsigned int POOL_SIZE = 10;
ObjectManager::ObjectManager()
{

}
ObjectManager::ObjectManager(World* newWorld, Cell* newParent)
{
    indexQuadTree = new QuadTree<Object*>(MAX_NODE_CAPACITY, 0, 0, CELL_WIDTH_PIXELS, CELL_HEIGHT_PIXELS);
    world = newWorld;
    parentCell = newParent;
}
ObjectManager::~ObjectManager()
{
    delete indexQuadTree;
}
ObjectPool* ObjectManager::getObjectPool(int type)
{
    std::map<int, ObjectPool >::iterator searcher =
    objectPools.find(type);
    if (searcher != objectPools.end())
    {
        return &searcher->second;
    }
    else return NULL;
}
ObjectPool* ObjectManager::createPool(int type, unsigned int size)
{
    ObjectPool emptyPool;
    objectPools[type] = emptyPool;
    ObjectPool* newPool = getObjectPool(type);
    if (!newPool) return NULL;
    newPool->pool.resize(size);
    newPool->firstEmptyObj = &newPool->pool[0];
    //Set all Object next indices to the next free object (next one over)
    for (unsigned int i = 0; i < size - 1; ++i)
    {
        Object* currentObject = &newPool->pool[i];
        //Use type to store whether or not object is in use
        currentObject->type = -1;
        //Use store next
        currentObject->_nextPoolObject = &newPool->pool[i + 1];
    }
    Object* lastObject = &newPool->pool[size - 1];
    //Use type to store whether or not object is in use
    lastObject->type = -1;
    //Last pool object
    lastObject->_nextPoolObject = NULL;
    return newPool;
}
//Returns a pointer to a vector with all objects in this
//manager of the requested type or NULL if there are no objects of
//the requested type
std::vector<Object*>* ObjectManager::getObjectsOfType(int type)
{
    std::vector<Object*>* objectsOfType = new std::vector<Object*>;
    ObjectPool* pool = getObjectPool(type);
    for (std::vector<Object>::iterator it = pool->pool.begin(); it!= pool->pool.end(); ++it)
    {
        //Make sure it is initialized
        if ((*it).type != -1)
        {
            objectsOfType->push_back(&(*it));
        }
    }
    return objectsOfType;
}
//Returns a pointer to an uninitialized (nor constructed) object
//or NULL if there are no free pool spaces. The object will be added
//to the quadtree at the specified position (but YOU MUST SET POSITION in obj)
Object* ObjectManager::getNewObject(int type, float x, float y)
{
    //Get the object pool
    ObjectPool* objects = getObjectPool(type);
    //Pool for this type has not been initialized, so initialize one
    if (!objects)
    {
        objects = createPool(type, POOL_SIZE);
        if (!objects) return NULL;
    }
    Object* firstEmptyObj = objects->firstEmptyObj;
    //Pool is full
    if (firstEmptyObj==NULL)
    {
        /*Resize pool does not work because vector will move data to
         *allocate storage, making all pointers invalid (big deal). If
         *resizing is desired, use a goddamn std::list, or use overflow
         *pools
         * //Resize pool
        int oldSize = objects->pool.size();
        objects->pool.resize(oldSize + POOL_SIZE);
        //Prepare pool data
        Object* oldFirstEmptyObj = objects->firstEmptyObj;
        objects->firstEmptyObj = &objects->pool[oldSize];
        for (unsigned int i = oldSize; i < objects->pool.size() - 1; ++i)
        {
            Object* currentObject = &objects->pool[i];
            //Use type to store whether or not object is in use
            currentObject->type = -1;
            //Use store next
            currentObject->_nextPoolObject = &objects->pool[i + 1];
        }
        Object* lastObject = &objects->pool[objects->pool.size() - 1];
        //Use type to store whether or not object is in use
        lastObject->type = -1;
        //Last pool object
        lastObject->_nextPoolObject = oldFirstEmptyObj;*/
        
        //Don't allow pool resizes
        std::cout << "WARNING: Pool at max size. Object cannot be created\n";
        return NULL;
    }
    Object* emptyObject = objects->firstEmptyObj;
    //Show this object is in use
    emptyObject->type=0;
    //Set the index to the next empty object
    objects->firstEmptyObj = emptyObject->_nextPoolObject;
    //Add object to quadtree at specified position
    indexQuadTree->insert(emptyObject, x, y);
    return emptyObject;
}
//Removes an object from the pool (must call this to work with pool)
//Type comes from object, so don't wipe the memory
void ObjectManager::removeObject(Object* objectToRemove)
{
    ObjectPool* objects = getObjectPool(objectToRemove->type);
    if (!objects)
    {
        std::cout << "WARNING: removeObject(): No pool found for type " << objectToRemove->type << "!\n";
        return;
    }
    //Remove the object pointer from the quadtree
    if (!indexQuadTree->remove(objectToRemove, objectToRemove->position.getCellOffsetX(), objectToRemove->position.getCellOffsetY()))
    {
        std::cout << "WARNING: QuadTree removal failed: object position does not match data position\n";
    }
    //Set the object to uninitialized
    objectToRemove->type = -1;
    objectToRemove->_nextPoolObject = objects->firstEmptyObj;
    objects->firstEmptyObj = objectToRemove;
}
void ObjectManager::moveObject(Object* objectToMove, Coord& newPosition)
{
    //Remove the object pointer from the quadtree
    if (!indexQuadTree->remove(objectToMove, objectToMove->position.getCellOffsetX(), objectToMove->position.getCellOffsetY()))
    {
        std::cout << "WARNING: QuadTree removal failed: object position does not match data position\n";
    }

    //Make sure object isn't colliding with anything
    //TODO
    Coord newDisplacement;
    CellIndex cell = newPosition.getCell();
    newDisplacement.setPosition(cell, newPosition.getCellOffsetX(), newPosition.getCellOffsetY());

    //Apply the displacement
    CellIndex displaceCell = newDisplacement.getCell();
    objectToMove->position.setPosition(displaceCell,
    newDisplacement.getCellOffsetX(), newDisplacement.getCellOffsetY());

    //Make sure object is still in this cell; if not, tell world and remove object
    //TODO
    
    //Reinsert object into quadtree
    if (!indexQuadTree->insert(objectToMove, newDisplacement.getCellOffsetX(), newDisplacement.getCellOffsetY()))
    {
        std::cout << "WARNING: moveObject(): QuadTree insertion failed!\n";
    }
}
#endif
