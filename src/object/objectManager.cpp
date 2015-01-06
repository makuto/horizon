#ifndef OBJECTMANAGER_CPP
#define OBJECTMANAGER_CPP
#include "objectManager.hpp"
#include <vector>
#include <iostream>
#include <base2.0/collision/collision.hpp>
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
//When moving an object, add COLL_SEARCH_TOLERANCE to the range the moving
//object could touch
const float COLL_SEARCH_TOLERANCE = 128;
//In order to prevent overlap, COLL_SKIN_THICKNESS is added to the moving object's
//AABB in moveObject. Adjust the graphics in order to hide any cracks
//(apparently Box2D uses this and it is therefore a good idea)
const float COLL_SKIN_THICKNESS = 3;
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
            removeObject(newObj);
            std::cout << "ERROR: getNewInitializedObject(): Obj Processor for type " << type << " not found!\n";
            return NULL;
        }
        Coord newPosition;
        newPosition.setPosition(parentCellID, x, y);
        //processor->initObject(newObj, subType, newObj->position, rotation, this);
        processor->initObject(newObj, subType, newPosition, rotation, this);
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
void ObjectManager::moveObject(Object* objectToMove, Coord& newPosition)
{
    //TODO: Make sure object isn't colliding with anything (using new displacement)
    Coord newDisplacement = newPosition;
    CellIndex displaceCell = newDisplacement.getCell();

    //Make sure object is still in this cell; if not, tell world and remove object
    //TODO: How will collision work if it will hit something in this cell before leaving?
    if (displaceCell.x != parentCellID.x || displaceCell.y != parentCellID.y)
    {
        Cell* cellAfterDisplace = parentCell->getNeighborCell(newDisplacement.getCell());
        if (!cellAfterDisplace) //Cell does not exist
        {
            std::cout << "ERROR: moveObject() - displace: Cell [ " << displaceCell.x << " , " <<
            displaceCell.y << " ] does not exist! Ignoring movement\n";
            //removeObject(objectToMove);
            return;
        }
        else if (cellAfterDisplace != parentCell) //Obj is no longer in this cell
        {
            //Get a object in the new cell
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
            *newObject = *objectToMove;
            //Apply displacement to new object
            newObject->position.setPosition(displaceCell,
            newDisplacement.getCellOffsetX(), newDisplacement.getCellOffsetY());
            //Remove migrated object from this manager
            removeObject(objectToMove);
            return;
        }
    }


    //Check for any objects this object will collide with
    aabb range;
    //Prepare search range (box from old position to new position)
    //TODO: Improve this
    //TODO: Make sure 0 returns are handled well (return new position?)
    /*if (newDisplacement.getCellOffsetX() < objectToMove->position.getCellOffsetX())
    {
        range.x = newDisplacement.getCellOffsetX() - COLL_SEARCH_TOLERANCE;
        range.w = abs(newDisplacement.getCellOffsetX() - newPosition.getCellOffsetX()) + objectToMove->boundOffsetX;
    }
    else
    {
        range.x = objectToMove->position.getCellOffsetX() - objectToMove->boundOffsetX + objectToMove->boundOffsetX;
        range.w = abs(newDisplacement.getCellOffsetX() - newPosition.getCellOffsetX()) + COLL_SEARCH_TOLERANCE;
    }
    if (newDisplacement.getCellOffsetY() < objectToMove->position.getCellOffsetY())
    {
        range.y = newDisplacement.getCellOffsetY() - COLL_SEARCH_TOLERANCE;
        range.h = newDisplacement.getCellOffsetY() + COLL_SEARCH_TOLERANCE + objectToMove->boundOffsetY + objectToMove->boundOffsetY;
    }
    else
    {
        range.y = objectToMove->position.getCellOffsetY() - COLL_SEARCH_TOLERANCE - objectToMove->boundOffsetY;
        range.h = newDisplacement.getCellOffsetY() + COLL_SEARCH_TOLERANCE;
    }*/
    float dispX = newDisplacement.getCellOffsetX();
    float dispY = newDisplacement.getCellOffsetY();
    float objX = objectToMove->position.getCellOffsetX();
    float objY = objectToMove->position.getCellOffsetY();
    if (dispX > objX)
    {
        range.x = objX;
        range.w = (dispX - objX) + COLL_SEARCH_TOLERANCE;
    }
    else if (dispX < objX)
    {
        range.w = (objX - dispX) + COLL_SEARCH_TOLERANCE;
        range.x = objX - range.w;
    }
    else
    {
        range.x = objX - COLL_SEARCH_TOLERANCE;
        range.w = COLL_SEARCH_TOLERANCE + COLL_SEARCH_TOLERANCE;
    }
    if (dispY > objY)
    {
        range.y = objY;
        range.h = (dispY - objY) + COLL_SEARCH_TOLERANCE;
    }
    else if (dispY < objY)
    {
        range.h = (objY - dispY) + COLL_SEARCH_TOLERANCE;
        range.y = objY - range.h;
    }
    else
    {
        range.y = objY - COLL_SEARCH_TOLERANCE;
        range.h = COLL_SEARCH_TOLERANCE + COLL_SEARCH_TOLERANCE;
    }
    /*range.x = objectToMove->position.getCellOffsetX();
    range.y = objectToMove->position.getCellOffsetY();
    range.w = newDisplacement.getCellOffsetX() - newPosition.getCellOffsetX() + objectToMove->boundOffsetX;
    range.h = newDisplacement.getCellOffsetY() - newPosition.getCellOffsetY() + objectToMove->boundOffsetY;*/
    //Use quadtree to find objects in range
    getObjectsInRangeCache(range);
    //Prepare this object's bounds
    aabb objToMoveBounds = objectToMove->bounds;
    objToMoveBounds.x = newDisplacement.getCellOffsetX() + objectToMove->boundOffsetX - COLL_SKIN_THICKNESS;
    objToMoveBounds.y = newDisplacement.getCellOffsetY() + objectToMove->boundOffsetY - COLL_SKIN_THICKNESS;
    objToMoveBounds.w = objectToMove->bounds.w + (COLL_SKIN_THICKNESS * 2);
    objToMoveBounds.h = objectToMove->bounds.h + (COLL_SKIN_THICKNESS * 2);
    //Get the object processor
    ObjectProcessor* movingObjProcessor = processorDir->getObjProcessor(objectToMove->type);
    //Skip all obj collisions if the processor wasn't found
    if (movingObjProcessor != NULL)
    {
        for (int i = 0; i < numQueryPoints; ++i)
        {
            Object* currentObj = queryArray[i];
            if (currentObj == objectToMove) continue; //Don't trigger self collision
            if (isColliding(&objToMoveBounds, &currentObj->bounds))
            {
                /*////////////////DEBUG COLLIDE
                window win(600, 600, "Test collide");
                text test;
                while(!win.shouldClose())
                {
                    test.loadFont("data/fonts/font1.ttf");
                    test.setSize(10);
                    float viewX = 200;
                    float viewY = 200;
                    Object* obj = objectToMove;
                    sf::RenderWindow* sfWin = win.getBase();
                    sf::RectangleShape rectangle;
                    rectangle.setSize(sf::Vector2f(obj->bounds.w, obj->bounds.h));
                    rectangle.setFillColor(sf::Color::Transparent);
                    rectangle.setOutlineColor(sf::Color::Red);
                    rectangle.setOutlineThickness(2);
                    rectangle.setPosition(obj->getPosition().getCellOffsetX() - viewX + obj->boundOffsetX, obj->getPosition().getCellOffsetY() - viewY + obj->boundOffsetY);
                    sfWin->draw(rectangle);
                    std::stringstream ss;
                    ss << obj->id;
                    test.setPosition(obj->getPosition().getCellOffsetX() - viewX + obj->boundOffsetX, obj->getPosition().getCellOffsetY() - viewY + obj->boundOffsetY);
                    test.setText(ss.str());
                    win.draw(&test);
                    
                    obj = currentObj;
                    rectangle.setSize(sf::Vector2f(obj->bounds.w, obj->bounds.h));
                    rectangle.setOutlineColor(sf::Color::Red);
                    rectangle.setOutlineThickness(2);
                    rectangle.setPosition(obj->getPosition().getCellOffsetX() - viewX + obj->boundOffsetX, obj->getPosition().getCellOffsetY() - viewY + obj->boundOffsetY);
                    test.setPosition(obj->getPosition().getCellOffsetX() - viewX + obj->boundOffsetX, obj->getPosition().getCellOffsetY() - viewY + obj->boundOffsetY);
                    sfWin->draw(rectangle);
                    ss.clear();
                    ss.str("");
                    ss << obj->id;
                    test.setText(ss.str());
                    win.draw(&test);
                    
                    rectangle.setSize(sf::Vector2f(range.w, range.h));
                    rectangle.setOutlineColor(sf::Color::Green);
                    rectangle.setOutlineThickness(1);
                    rectangle.setPosition(range.x - viewX, range.y - viewY);
                    sfWin->draw(rectangle);
                    rectangle.setSize(sf::Vector2f(1, 1));
                    rectangle.setOutlineColor(sf::Color::Green);
                    rectangle.setPosition(newDisplacement.getTrueX() - viewX, newDisplacement.getTrueY() - viewY);
                    sfWin->draw(rectangle);
                    rectangle.setOutlineColor(sf::Color::White);
                    rectangle.setPosition(objectToMove->position.getTrueX() - viewX, objectToMove->position.getTrueY() - viewY);
                    sfWin->draw(rectangle);

                    viewX = 0;
                    viewY = 0;
                    rectangle.setOutlineColor(sf::Color::Blue);
                    obj = objectToMove;
                    rectangle.setPosition(obj->bounds.x - viewX + obj->boundOffsetX, obj->bounds.y - viewY + obj->boundOffsetY);
                    rectangle.setSize(sf::Vector2f(obj->bounds.w, obj->bounds.h));
                    sfWin->draw(rectangle);
                    ss.clear();
                    ss.str("");
                    ss << obj->id;
                    test.setPosition(obj->getPosition().getCellOffsetX() - viewX + obj->boundOffsetX, obj->getPosition().getCellOffsetY() - viewY + obj->boundOffsetY);
                    test.setText(ss.str());
                    win.draw(&test);
                    
                    obj = currentObj;
                    rectangle.setSize(sf::Vector2f(obj->bounds.w, obj->bounds.h));
                    rectangle.setFillColor(sf::Color::Transparent);
                    rectangle.setOutlineColor(sf::Color::Blue);
                    rectangle.setOutlineThickness(2);
                    rectangle.setPosition(obj->bounds.x - viewX + obj->boundOffsetX, obj->bounds.y - viewY + obj->boundOffsetY);
                    rectangle.setSize(sf::Vector2f(obj->bounds.w, obj->bounds.h));
                    test.setPosition(obj->getPosition().getCellOffsetX() - viewX + obj->boundOffsetX, obj->getPosition().getCellOffsetY() - viewY + obj->boundOffsetY);
                    sfWin->draw(rectangle);
                    ss.clear();
                    ss.str("");
                    ss << obj->id;
                    test.setText(ss.str());
                    win.draw(&test);
                    win.update();
                }
                ////////////////DEBUG COLLIDE*/
                
                //Grab the object processors
                ObjectProcessor* stationaryObjProcessor = processorDir->getObjProcessor(currentObj->type);
                //Skip this object's collision if its processor wasn't found
                if (stationaryObjProcessor==NULL) continue;
                int movingReturn = movingObjProcessor->onCollideObj(objectToMove, newDisplacement, currentObj, true);
                int stationaryReturn = stationaryObjProcessor->onCollideObj(currentObj, newDisplacement, objectToMove, false);
                //Do different things based on the return codes
                switch(movingReturn)
                {
                    case 0: //The ObjectProcessor handled it
                        if (stationaryReturn==2) //stationaryObj might still want to be destroyed
                        {
                            stationaryObjProcessor->onDestroyObject(currentObj);
                            removeObject(currentObj);
                        }
                        break;
                    case 1: //Resolve the collision
                        switch(stationaryReturn)
                        {
                            case 0: //StationaryObj handled it
                                break;
                            case 1: //Resolve the collision normally (keep stationary stationary)
                                {
                                    //TODO: Replace with better algorithm
                                    
                                    /*vec2 vec;
                                    vec.x = newDisplacement.getCellOffsetX() - objectToMove->position.getCellOffsetX();
                                    vec.y = newDisplacement.getCellOffsetY() - objectToMove->position.getCellOffsetY();
                                    resolveCollision(&objToMoveBounds, &currentObj->bounds, &vec);
                                    float newX = objToMoveBounds.x - objectToMove->boundOffsetX;
                                    float newY = objToMoveBounds.y - objectToMove->boundOffsetY;
                                    //TODO: Is it OK to have parentCellID like that? What if it leaves cell?
                                    //objectToMove->position.setPosition(parentCellID, newX, newY);
                                    newDisplacement.setPosition(parentCellID, newX, newY);*/
                                    //Simply ignore the new displacement
                                    newDisplacement.setPosition(parentCellID, objectToMove->position.getCellOffsetX(),
                                    objectToMove->position.getCellOffsetY());
                                }
                                break;
                            //if stationaryObj wants to be destroyed, then that's
                            //all that has to be done to resolve the collision
                            case 2:
                                stationaryObjProcessor->onDestroyObject(currentObj);
                                removeObject(currentObj);
                                break;
                            case 3: //Resolve the collision by moving stationaryObj
                                //TODO: If pushing against a wall etc, could this
                                //recurse forever and break everything?
                                {
                                    vec2 vec;
                                    vec.x = objectToMove->position.getCellOffsetX() - newDisplacement.getCellOffsetX();
                                    vec.y = objectToMove->position.getCellOffsetY() - newDisplacement.getCellOffsetY();
                                    resolveCollision(&currentObj->bounds, &objToMoveBounds, &vec);
                                    float newX = currentObj->bounds.x - currentObj->boundOffsetX;
                                    float newY = currentObj->bounds.y - currentObj->boundOffsetY;
                                    //TODO: Is it OK to have parentCellID like that? What if it leaves cell?
                                    Coord newPosition;
                                    newPosition.setPosition(parentCellID, newX, newY);
                                    //Go through objMan to set position
                                    std::cout << "objMan\n";
                                    currentObj->setPosition(newPosition, *this);
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case 2: //Destroy the object
                        movingObjProcessor->onDestroyObject(objectToMove);
                        removeObject(objectToMove);
                        return;
                        break;
                    case 3: //Move stationaryObj out of the way
                        switch(stationaryReturn)
                        {
                            case 0: //StationaryObj handled it
                                break;
                            //if stationaryObj wants to be destroyed, then that's
                            //all that has to be done to resolve the collision
                            case 2:
                                stationaryObjProcessor->onDestroyObject(currentObj);
                                removeObject(currentObj);
                                break;
                            //Resolve the collision by moving stationaryObj
                            //TODO: If pushing against a wall etc, could this
                            //recurse forever and break everything?
                            //Regardless of stationaryObj's return, moving takes
                            //precedence
                            default:
                                {
                                    vec2 vec;
                                    vec.x = objectToMove->position.getCellOffsetX() - newDisplacement.getCellOffsetX();
                                    vec.y = objectToMove->position.getCellOffsetY() - newDisplacement.getCellOffsetY();
                                    resolveCollision(&currentObj->bounds, &objToMoveBounds, &vec);
                                    float newX = currentObj->bounds.x - currentObj->boundOffsetX;
                                    float newY = currentObj->bounds.y - currentObj->boundOffsetY;
                                    //TODO: Is it OK to have parentCellID like that? What if it leaves cell?
                                    Coord newPosition;
                                    newPosition.setPosition(parentCellID, newX, newY);
                                    //Go through objMan to set position
                                    std::cout << "objMan\n";
                                    currentObj->setPosition(newPosition, *this);
                                }
                                break;
                        }
                        break;
                    default: //Unknown return code
                        break;
                }
            }
        }
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
    /*aabb range2(0, 0, 2048, 2048);
    std::vector<Object*> vec;
    std::cout << indexQuadTree->queryRange(range2, vec) << "\n";*/
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
    indexQuadTree->render(win, -viewX, -viewY);
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
