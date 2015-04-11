#ifndef PICKUP_OBJECTPROCESSOR_CPP
#define PICKUP_OBJECTPROCESSOR_CPP
#include "pickupObjectProcessor.hpp"
#include "../objectManager.hpp"

PickupObjectProcessor::PickupObjectProcessor()
{
    processorType = -1;
}
PickupObjectProcessor::~PickupObjectProcessor()
{
}

void PickupObjectProcessor::setup(ItemManager* newItemManager)
{
    itemManager = newItemManager;
}

bool PickupObjectProcessor::initialize(eptFile* spec)
{
    processorType = attrToInt(spec->getAttribute("defaults.type"));
    return true;
}
//--Overload functions for object dynamic behavior
//--Functions should return -1 if the object should be destroyed
//Called to initialize a new-ed object - use this function to make sure
//objects are valid subtypes in valid positions
//Return false if the object cannot be created
bool PickupObjectProcessor::initObject(Object* newObj, int subType, Coord& position, float rotation, ObjectManager* manager)
{
    newObj->subType = subType;
    newObj->setPosition(position, *manager);
    newObj->rotation = rotation;
    newObj->lastUpdate.reset();
    newObj->bounds.w = 30;
    newObj->bounds.h = 30;
    newObj->boundOffsetX = -15;
    newObj->boundOffsetY = -15;
    newObj->manhattanRadius = 1000;
    newObj->bounds.setPosition(position.getCellOffsetX() + newObj->boundOffsetX, position.getCellOffsetY() + newObj->boundOffsetY);
    return true;
}
//Do a routine update on the object
int PickupObjectProcessor::updateObject(Object* obj, Time* globalTime, ObjectManager* manager)
{
    Time delta;
    obj->lastUpdate.getDeltaTime(globalTime, delta);
    obj->lastUpdate = *globalTime;
    return 1;
}
//Render the object (it is in view of player)
void PickupObjectProcessor::renderObject(Object* obj, float viewX, float viewY, window* win)
{
    Coord pos = obj->getPosition();
    ItemProcessor* thisItemProcessor = itemManager->getItemProcessor(obj->subType);
    if (!thisItemProcessor) return;
    Item item;
    item.type = obj->subType;
    item.subType = obj->state;
    thisItemProcessor->renderItem(&item, pos, viewX, viewY, 0, win);
}
/*//Agent uses/activates object
int PickupObjectProcessor::activateObject(Object* obj, Agent* agent)
{
    return 1;
}

//Agent collides with object
int PickupObjectProcessor::touchObject(Object* collider, Coord& collideDisplacement, Agent* agent, bool isMoving)
{
    return 1;
}*/
//Object collides with tile
int PickupObjectProcessor::onCollideTile(Object* collider, Coord& collideDisplacement, tile* touchedTile)
{
    return 1;
}
//Object collides with object
int PickupObjectProcessor::onCollideObj(Object* collider, Coord& collideDisplacement, Object* obj, bool isMoving)
{
    return 1;
}

//Object is hit by something
//virtual int hitObject(Object* obj, Actor* actor, int inventoryIndex); //TODO
//This function is called when an object is going to be destroyed
//Use this function to do any cleanup if needed. The Object itself
//should NOT be deleted by this function
void PickupObjectProcessor::onDestroyObject(Object* obj)
{
    return;
}
#endif
