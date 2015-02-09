#ifndef OBJECTMANAGER_CPP
#define OBJECTMANAGER_CPP
#include "objectManager.hpp"
#include <vector>
#include <iostream>
#include <base2.0/collision/collision.hpp>
#include <base2.0/math/math.hpp>
#include "../world/cell.hpp"

//TEMP
#include <sstream>



//For quadtree
const unsigned int MAX_NODE_CAPACITY = 3;
//The size of each object pool
const unsigned int POOL_SIZE = 100;
//Should equal largest object sprite half width (used to figure out what
//objects are in the view and should be rendered); object centers not in
//view size + VIEW_TOLERANCE will be culled
const float VIEW_TOLERANCE = 32;
//Use this in conjunction with NUM_DEFAULT_POOLS to pre-init object pools
//The number is the type of object to pool
const int DEFAULT_POOLS[] = {1};
const int NUM_DEFAULT_POOLS = 1;
//When moving an object, COLL_SEARCH_TOLERANCE is added to the range the moving
//object could touch; this variable is also used for querying nearby cells
//for neighborhood collision detection
const float COLL_SEARCH_TOLERANCE = 96; //128
//In order to prevent overlap, COLL_SKIN_THICKNESS is added to the moving object's
//AABB in moveObject. Adjust the graphics in order to hide any cracks
//(apparently Box2D uses this and it is therefore a good idea)
//See pg 17 http://box2d.org/manual.pdf
const float COLL_SKIN_THICKNESS = 0; //3
//Box2D is designed for smaller units, so pixels aren't good. All sizes/positions
//will be divided by BOX2D_SCALE when doing collision functions so that the units
//work better with Box2D.
const float BOX2D_SCALE = 8;
//For getObjectsInRangeCache, this is the size of the results array
const int MAX_QUERY_POINTS = 50;

ObjectManager::ObjectManager(World* newWorld, ObjectProcessorDir* newProcessorDir, CellIndex newParentCellID, Cell* newParent):parentCellID(newParentCellID)
{
    indexQuadTree = new QuadTree<Object*>(MAX_NODE_CAPACITY, 0, 0, CELL_WIDTH_PIXELS, CELL_HEIGHT_PIXELS);
    world = newWorld;
    parentCell = newParent;
    //parentCellID = newParentCellID;
    processorDir = newProcessorDir;
    for (int i = 0; i < NUM_DEFAULT_POOLS; ++i)
    {
        createPool(DEFAULT_POOLS[i], POOL_SIZE);
    }
    indexQuadTree = NULL;
    queryArray = new Object*[MAX_QUERY_POINTS];
    numQueryPoints = 0;
}
ObjectManager::ObjectManager()
{
    world = NULL;
    parentCell = NULL;
    processorDir = NULL;
    parentCellID.x = -1;
    parentCellID.y = -1;
    
    for (int i = 0; i < NUM_DEFAULT_POOLS; ++i)
    {
        createPool(DEFAULT_POOLS[i], POOL_SIZE);
    }
    indexQuadTree = NULL;
    queryArray = new Object*[MAX_QUERY_POINTS];
    numQueryPoints = 0;
}
void ObjectManager::init(World* newWorld, ObjectProcessorDir* newProcessorDir,
CellIndex newParentCellID, Cell* newParent)
{
    //Wipe quadtree
    if (indexQuadTree != NULL)
    {
        delete indexQuadTree;
    }
    //Wipe pools
    for (std::map<int, ObjectPool>::iterator it = objectPools.begin();
    it != objectPools.end(); ++it)
    {
        it->second.firstEmptyObj = &it->second.pool[0];
        //Set all Object next indices to the next free object (next one over)
        for (unsigned int i = 0; i < POOL_SIZE - 1; ++i)
        {
            Object* currentObject = &it->second.pool[i];
            //Use type to store whether or not object is in use
            currentObject->type = -1;
            //Use store next
            currentObject->_nextPoolObject = &it->second.pool[i + 1];
        }
        Object* lastObject = &it->second.pool[POOL_SIZE - 1];
        //Use type to store whether or not object is in use
        lastObject->type = -1;
        //Last pool object
        lastObject->_nextPoolObject = NULL;
    }
    
    indexQuadTree = new QuadTree<Object*>(MAX_NODE_CAPACITY, 0, 0, CELL_WIDTH_PIXELS, CELL_HEIGHT_PIXELS);
    world = newWorld;
    parentCell = newParent;
    parentCellID = newParentCellID;
    processorDir = newProcessorDir;

    numQueryPoints = 0;
}
ObjectManager::~ObjectManager()
{
    delete indexQuadTree;
    delete[] queryArray;
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
    //newPool->pool.reserve(size);
    //std::cout << "pool size: " << newPool->pool.size() * sizeof(Object) << "\n";
    newPool->firstEmptyObj = &newPool->pool[0];
    //Set all Object next indices to the next free object (next one over)
    int counter = 1;
    for (unsigned int i = 0; i < size - 1; ++i)
    {
        //newPool->pool.push_back(newObject);
        Object* currentObject = &newPool->pool[i];
        //Use type to store whether or not object is in use
        currentObject->type = -1;
        //Use store next
        currentObject->_nextPoolObject = &newPool->pool[i + 1];
        counter++;
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
std::vector<Object*>* ObjectManager::getObjectsInRange(aabb& range)
{
    //TODO: pool this array
    std::vector<Object*>* objectsInRange = new std::vector<Object*>;
    indexQuadTree->queryRange(range, *objectsInRange);
    return objectsInRange;
}
void ObjectManager::getObjectsInRangeCache(aabb& range)
{
    numQueryPoints = 0;
    numQueryPoints = indexQuadTree->queryRange(range, queryArray, numQueryPoints, MAX_QUERY_POINTS);
}
//Returns a pointer to an uninitialized (nor constructed) object
//or NULL if there are no free pool spaces. The object will be added
//to the quadtree at the specified position. position will be set to
//This current cell and x, y; type will also be set
Object* ObjectManager::getNewRawObject(int type, float x, float y)
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
    //Set the index to the next empty object
    objects->firstEmptyObj = emptyObject->_nextPoolObject;
    //Add object to quadtree at specified position
    indexQuadTree->insert(emptyObject, x, y);
    //Set the object type to the type passed in (dually set to in use)
    emptyObject->type=type;
    //TODO Set the object id
    emptyObject->id = rand() % 100000;
    //Set the object position to this cell + x y offsets
    emptyObject->position.setPosition(parentCellID, x, y);
    return emptyObject;
}
//Uses getNewRawObject and calls initObject() for the object's type
Object* ObjectManager::getNewInitializedObject(int type, int subType, float x, float y, float rotation)
{
    Object* newObj = getNewRawObject(type, x, y);
    if (newObj)
    {
        ObjectProcessor* processor = processorDir->getObjProcessor(type);
        if (!processor)
        {
            std::cout << "ERROR: getNewInitializedObject(): Obj Processor for type " << type << " not found!\n";
            removeObject(newObj);
            return NULL;
        }
        Coord newPosition;
        newPosition.setPosition(parentCellID, x, y);
        //processor->initObject(newObj, subType, newObj->position, rotation, this);
        if (!processor->initObject(newObj, subType, newPosition, rotation, this))
        {
            std::cout << "WARNING: getNewInitiallizedObject(): Object of type " << type << " returned failed initialization.\n";
            removeObject(newObj);
            return NULL;
        }
        return newObj;
    }
    return NULL;
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
        std::cout << "WARNING: QuadTree removal failed: object position does not match data position:\n";
        objectToRemove->position.print();
        std::cout << "(In cell " << parentCellID.x << " , " << parentCellID.y << ")\n";
    }
    //Set the object to uninitialized
    objectToRemove->type = -1;
    objectToRemove->id = 0;
    objectToRemove->_nextPoolObject = objects->firstEmptyObj;
    objects->firstEmptyObj = objectToRemove;
}


//TODO: [FIXED via getRelativeCell] Is getTrueX and Y safe? It is
//possible to convert to a local offset in this case?
Coord ObjectManager::preventObjectCollisions(Object* objectToMove, Coord& newPosition)
{
    Coord newDisplacement = newPosition;

    //Check for any objects this object will collide with
    aabb range;
    float objX = objectToMove->position.getRelativeCellX(parentCellID);
    float objY = objectToMove->position.getRelativeCellY(parentCellID);
    //Prepare search range (box around position with COLL_SEARCH_TOLERANCE as halfwidth)
    //TODO: Improve this
    //Use quadtree to find objects in range
    range.x = objX - COLL_SEARCH_TOLERANCE;
    range.y = objY - COLL_SEARCH_TOLERANCE;
    range.w = COLL_SEARCH_TOLERANCE * 2;
    range.h = COLL_SEARCH_TOLERANCE * 2;
    getObjectsInRangeCache(range);
    
    //Prepare this object's bounds
    aabb objToMoveBounds = objectToMove->bounds;
    objToMoveBounds.x = newDisplacement.getRelativeCellX(parentCellID) + objectToMove->boundOffsetX - COLL_SKIN_THICKNESS;
    objToMoveBounds.y = newDisplacement.getRelativeCellY(parentCellID) + objectToMove->boundOffsetY - COLL_SKIN_THICKNESS;
    objToMoveBounds.w = objectToMove->bounds.w + (COLL_SKIN_THICKNESS * 2);
    objToMoveBounds.h = objectToMove->bounds.h + (COLL_SKIN_THICKNESS * 2);
    
    //Get the object processor
    ObjectProcessor* movingObjProcessor = processorDir->getObjProcessor(objectToMove->type);
    //Skip all obj collisions if the processor wasn't found
    if (movingObjProcessor != NULL)
    {
        //Loop through all objects in range
        for (int i = 0; i < numQueryPoints; ++i)
        {
            Object* currentObj = queryArray[i];
            if (currentObj == objectToMove)
            {
                continue; //Don't trigger self collision
            }
            
            float dispXa = newDisplacement.getRelativeCellX(parentCellID);
            float dispYa = newDisplacement.getRelativeCellY(parentCellID);
            float objXa = objectToMove->position.getRelativeCellX(parentCellID);
            float objYa = objectToMove->position.getRelativeCellY(parentCellID);
            
            float velX = (dispXa - objXa);
            float velY = (dispYa - objYa);

            //Don't test for collisions if the velocity is 0
            if (velX == 0 && velY == 0)
            {
                break;
            }

            //Grab the current object's processor
            ObjectProcessor* stationaryObjProcessor = processorDir->getObjProcessor(currentObj->type);
            //Skip this object's collision if its processor wasn't found
            if (stationaryObjProcessor==NULL)
            {
                continue;
            }

            //Check if the object is on a collision course; notify objects
            int movingReturn = 1;
            int stationaryReturn = 1;
            if (manhattanTo(objXa, objYa, currentObj->position.getRelativeCellX(parentCellID), currentObj->position.getRelativeCellY(parentCellID)) <= objectToMove->manhattanRadius + currentObj->manhattanRadius)
            {
                movingReturn = movingObjProcessor->onCollideObj(objectToMove, newDisplacement, currentObj, true);
                stationaryReturn = stationaryObjProcessor->onCollideObj(currentObj, newDisplacement, objectToMove, false);
            }
            //Object said it would handle it (by changing newDisplacement)
            if (movingReturn==0 || stationaryReturn==0) continue;
            //Object wants to be destroyed (resolving the collision)
            if (movingReturn==-1)
            {
                movingObjProcessor->onDestroyObject(objectToMove);
                removeObject(objectToMove);
                return newDisplacement; //If the moving object is destroyed, no more detection is necessary
            }
            //Stationary wants to be destroyed (resolving the collision)
            if (stationaryReturn==-1)
            {
                stationaryObjProcessor->onDestroyObject(currentObj);
                removeObject(currentObj);
                continue;
            }
            //Return codes are 1; prevent the collision
            
            //Get a new velocity if the object is on a collision course
            float newVX = 0;
            float newVY = 0;
            preventCollision(objToMoveBounds, currentObj->bounds, velX, velY, newVX, newVY, false);
            
            newDisplacement = objectToMove->position;
            newDisplacement.addVector(newVX, newVY);
            //NEW ADDITION
            objToMoveBounds.x = newDisplacement.getRelativeCellX(parentCellID) + objectToMove->boundOffsetX - COLL_SKIN_THICKNESS;
            objToMoveBounds.y = newDisplacement.getRelativeCellY(parentCellID) + objectToMove->boundOffsetY - COLL_SKIN_THICKNESS;
            //NEW ADDITION (end)
            if (newVX == 0 && newVY == 0)
            {
                break; //No more movement, so no more collisions need testing
            }
        }
    }

    //Return the final adjusted displacement
    return newDisplacement;
}
Coord ObjectManager::preventTileCollisions(Object* objectToMove, Coord& newPosition)
{
    Coord newDisplacement = newPosition;

    //Check for any objects this object will collide with
    float objX = objectToMove->position.getRelativeCellX(parentCellID);
    float objY = objectToMove->position.getRelativeCellY(parentCellID);
    
    //Prepare tile search range (box around position with COLL_SEARCH_TOLERANCE as halfwidth)
    int topX = parentCell->pointToTileValue(objX - COLL_SEARCH_TOLERANCE, true);
    int topY = parentCell->pointToTileValue(objY - COLL_SEARCH_TOLERANCE, false);
    int tileTolerance = parentCell->pointToTileValue(COLL_SEARCH_TOLERANCE, true);
    int bottomX = topX + (tileTolerance * 2);
    int bottomY = topY + (tileTolerance * 2);
    //Range checking
    if (bottomX > CELL_WIDTH) bottomX = CELL_WIDTH;
    if (bottomX < 0) bottomX = 0;
    if (bottomY > CELL_HEIGHT) bottomY = CELL_HEIGHT;
    if (bottomY < 0) bottomY = 0;
    /*if (objectToMove->id == 49941)
    {
        std::cout << "obj tile pos " << parentCell->pointToTileValue(objX, true) << " , " << parentCell->pointToTileValue(objY, false) << " search: " << topX << " , " << topY <<
        " to " << bottomX << " , " << bottomY << "\n";
    }*/
    if (topX > CELL_WIDTH) topX = CELL_WIDTH;
    if (topX < 0) topX = 0;
    if (topY > CELL_HEIGHT) topY = CELL_HEIGHT;
    if (topY < 0) topY = 0;
    
    //Prepare this object's bounds
    aabb objToMoveBounds = objectToMove->bounds;
    //Note that there is no skin for tile collisions
    objToMoveBounds.x = newDisplacement.getRelativeCellX(parentCellID) + objectToMove->boundOffsetX;
    objToMoveBounds.y = newDisplacement.getRelativeCellY(parentCellID) + objectToMove->boundOffsetY;
    objToMoveBounds.w = objectToMove->bounds.w;
    objToMoveBounds.h = objectToMove->bounds.h;
    
    //Get the object processor
    ObjectProcessor* movingObjProcessor = processorDir->getObjProcessor(objectToMove->type);
    //Skip all obj collisions if the processor wasn't found
    if (movingObjProcessor != NULL)
    {
        //Loop through all onground tiles in range
        for (int i = topY; i < bottomY; ++i)
        {
            for (int n = topX; n < bottomX; ++n)
            {
                tile* currentTile = parentCell->getTileAt(n, i, ONGROUND_LAYER);
                if (!currentTile) continue; //For some reason, the tile is NULL
                //Empty ground
                if (currentTile->x == 255 && currentTile->y == 255) continue;
                
                float dispXa = newDisplacement.getRelativeCellX(parentCellID);
                float dispYa = newDisplacement.getRelativeCellY(parentCellID);
                float objXa = objectToMove->position.getRelativeCellX(parentCellID);
                float objYa = objectToMove->position.getRelativeCellY(parentCellID);
                float tileX = n * TILE_WIDTH;
                float tileY = i * TILE_HEIGHT;
                
                float velX = (dispXa - objXa);
                float velY = (dispYa - objYa);
    
                //Don't test for collisions if the velocity is 0
                if (velX == 0 && velY == 0)
                {
                    break;
                }
    
                //Check if the object is on a collision course; notify objects
                int movingReturn = 1;
                if (manhattanTo(objXa, objYa, tileX + (TILE_WIDTH / 2), tileY + (TILE_HEIGHT / 2)) <= objectToMove->manhattanRadius + TILE_MANHATTAN_RADIUS)
                {
                    movingReturn = movingObjProcessor->onCollideTile(objectToMove, newDisplacement, currentTile);
                }
                //Object said it would handle it (by changing newDisplacement)
                if (movingReturn==0) continue;
                //Object wants to be destroyed (resolving the collision)
                if (movingReturn==-1)
                {
                    movingObjProcessor->onDestroyObject(objectToMove);
                    removeObject(objectToMove);
                    return newDisplacement; //If the moving object is destroyed, no more detection is necessary
                }
                
                //Return codes are 1; prevent the collision
                //Prepare the tile's bounds
                aabb tileBounds(tileX, tileY, TILE_WIDTH, TILE_HEIGHT);
                //Get a new velocity if the object is on a collision course
                float newVX = 0;
                float newVY = 0;
                preventCollision(objToMoveBounds, tileBounds, velX, velY, newVX, newVY, false);
                
                newDisplacement = objectToMove->position;
                newDisplacement.addVector(newVX, newVY);
                //NEW ADDITION
                objToMoveBounds.x = newDisplacement.getRelativeCellX(parentCellID) + objectToMove->boundOffsetX;
                objToMoveBounds.y = newDisplacement.getRelativeCellY(parentCellID) + objectToMove->boundOffsetY;
                //NEW ADDITION (end)
                if (newVX == 0 && newVY == 0)
                {
                    break; //No more movement, so no more collisions need testing
                }
            }
        }
    }

    //Return the final adjusted displacement
    return newDisplacement;
}
void ObjectManager::moveObject(Object* objectToMove, Coord& newPosition)
{
    Coord newDisplacement = newPosition;
    
    //Check if the object will collide with any other objects and adjust displacement
    newDisplacement = preventObjectCollisions(objectToMove, newDisplacement);
    newDisplacement = preventTileCollisions(objectToMove, newDisplacement);
    //For all cells in COLL_SEARCH_TOLERANCE, prevent collisions with
    //tiles or objects (make sure neighbor cells are being accounted for)
    int cellsReturned = 0;
    Coord topLeftRange = objectToMove->position;
    topLeftRange.addVector(-COLL_SEARCH_TOLERANCE, -COLL_SEARCH_TOLERANCE);
    CellIndex* intersectingCells = parentCell->getWorldIntersectingCells(topLeftRange,
    COLL_SEARCH_TOLERANCE * 2, COLL_SEARCH_TOLERANCE * 2, cellsReturned);
    for (int i = 0; i < cellsReturned; ++i)
    {
        //Skip the cell this object is in
        if (intersectingCells[i].x == parentCellID.x && intersectingCells[i].y == parentCellID.y) continue;
        //Get the cell
        Cell* currentCell = parentCell->getNeighborCell(intersectingCells[i]);
        if (!currentCell)
        {
            std::cout << "WARNING: moveObject() - nearby collision: Cell [ " << intersectingCells[i].x << " , " <<
            intersectingCells[i].y << " ] does not exist! Ignoring movement\n";
            continue;
        }
        //Check if the object will collide with any other objects or tiles and adjust displacement
        newDisplacement = currentCell->getObjectManager()->preventObjectCollisions(objectToMove, newDisplacement);
        newDisplacement = currentCell->getObjectManager()->preventTileCollisions(objectToMove, newDisplacement);
    }
    
    //TODO: [FIXED] How will collision work if it will hit something in this cell before leaving?
    //Make sure object is still in this cell; if not, tell world and remove object
    CellIndex displaceCell = newDisplacement.getCell();
    if (displaceCell.x != parentCellID.x || displaceCell.y != parentCellID.y)
    {
        Cell* cellAfterDisplace = parentCell->getNeighborCell(displaceCell);
        if (!cellAfterDisplace) //Cell does not exist
        {
            std::cout << "ERROR: moveObject() - displace: Cell [ " << displaceCell.x << " , " <<
            displaceCell.y << " ] does not exist! Ignoring movement\n";
            //removeObject(objectToMove);
            return;
        }
        //Get a object in the new cell that the migrating object will become
        ObjectManager* newManager = cellAfterDisplace->getObjectManager();
        Object* newObject = newManager->getNewRawObject(objectToMove->type,
        newDisplacement.getCellOffsetX(), newDisplacement.getCellOffsetY());
        if (newObject == NULL)
        {
            std::cout << "ERROR: moveObject(): no new object returned from getNewRawObject(), removing obj\n";
            removeObject(objectToMove);
            return;
        }
        //Copy this object to the other cell object
        *newObject = *objectToMove; //TODO: This is a big operation!!!
        //Apply displacement to new object
        newObject->position.setPosition(displaceCell,
        newDisplacement.getCellOffsetX(), newDisplacement.getCellOffsetY());
        //Remove migrated object from this manager
        removeObject(objectToMove);
        return;
    }

    //Object still in this cell; remove it from quadtree, then apply the
    //displacement and reinsert it into the quadtree
    //Remove the object pointer from the quadtree
    if (!indexQuadTree->remove(objectToMove, objectToMove->position.getCellOffsetX(), objectToMove->position.getCellOffsetY()))
    {
        std::cout << "WARNING: moveObject(): QuadTree removal failed!\n";
    }
    
    //Apply the displacement
    objectToMove->position.setPosition(displaceCell,
    newDisplacement.getCellOffsetX(), newDisplacement.getCellOffsetY());
    //Reinsert object into quadtree
    if (!indexQuadTree->insert(objectToMove, newDisplacement.getCellOffsetX(), newDisplacement.getCellOffsetY()))
    {
        std::cout << "WARNING: moveObject(): QuadTree insertion failed!\n";
    }
}
void ObjectManager::renderObjects(float viewX, float viewY, window* win)
{
    //Construct view bounds
    aabb view(viewX - VIEW_TOLERANCE, viewY - VIEW_TOLERANCE,
    win->getWidth() + (VIEW_TOLERANCE * 2), win->getHeight() + (VIEW_TOLERANCE * 2));
    //Get visible objects
    std::vector<Object*> visibleObjs;
    int resultCount = indexQuadTree->queryRange(view, visibleObjs);
    if (resultCount > 0)
    {
        ObjectProcessor* processor = NULL;
        int lastType = -1;
        for (std::vector<Object*>::iterator it = visibleObjs.begin();
        it != visibleObjs.end(); ++it)
        {
            Object* currentObject = (*it);
            if (currentObject != NULL)
            {
                //Find this object's processor (unless it's the same as last obj)
                if (processor==NULL || currentObject->type != lastType)
                {
                    //Find associated processor
                    processor = processorDir->getObjProcessor(currentObject->type);
                    //Skip object if its processor wasn't found
                    if (processor==NULL) continue;
                    //Successfully found processor; save its type in case next obj is that type
                    lastType = currentObject->type;
                }
                
                processor->renderObject(currentObject, viewX, viewY, win);
            }
        }
    }
    //Debug render quadtree
    //indexQuadTree->render(win, -viewX, -viewY);
}
bool ObjectManager::updateObjects(Time* globalTime)
{
    bool hasObject = false;
    ObjectProcessor* processor = NULL;
    for (std::map<int, ObjectPool>::iterator it = objectPools.begin();
    it != objectPools.end(); ++it)
    {
        int currentType = it->first;
        //Find this pool's processor
        processor = processorDir->getObjProcessor(currentType);
        //Skip object if its processor wasn't found
        if (processor==NULL) continue;
        //TODO: Only update objects that need to be updated
        for (std::vector<Object>::iterator pIt = it->second.pool.begin();
        pIt != it->second.pool.end(); ++pIt)
        {
            Object* currentObject = &(*pIt);
            if (currentObject->type != -1) //Skip over uninitialized objects
            {
                hasObject = true;
                int status = processor->updateObject(currentObject, globalTime, this);
                if (status == -1) //object should be destroyed
                {
                    processor->onDestroyObject(currentObject);
                    removeObject(currentObject);
                }
            }
        }
    }
    return hasObject;
}
#endif
