#ifndef OBJECTPROCESSOR_CPP
#define OBJECTPROCESSOR_CPP
#include "objectProcessor.hpp"
ObjectProcessor::ObjectProcessor()
{
    processorType = -1;
}
ObjectProcessor::~ObjectProcessor()
{
}

int ObjectProcessor::getType()
{
    return processorType;
}
void ObjectProcessor::initialize(eptFile* spec)
{
    eptGroup* defaultsGroup = spec->getGroup("defaults");
    processorType = attrToInt(defaultsGroup->getAttribute("id"));
    return;
}
//--Overload functions for object dynamic behavior
//--Functions should return -1 if the object should be destroyed
//Called to initialize a new-ed object - use this function to make sure
//objects are valid subtypes in valid positions
//Return false if the object cannot be created
bool ObjectProcessor::createObject(Object* newObj, int subType, Coord& position, float rotation)
{
    newObj->subType = subType;
    CellIndex newCell = position.getCell();
    newObj->position.setPosition(newCell, position.getCellOffsetX(), position.getCellOffsetY());
    newObj->rotation = rotation;
    return true;
}
//Do a routine update on the object
int ObjectProcessor::updateObject(Object* obj)
{
    return 1;
}
//Render the object (it is in view of player)
void ObjectProcessor::renderObject(Object* obj)
{
    return;
}
//Agent uses/activates object
int ObjectProcessor::activateObject(Object* obj, Agent* agent)
{
    return 1;
}
//Agent collides with object
int ObjectProcessor::touchObject(Object* obj, Agent* agent)
{
    return 1;
}
//Object is hit by something
//virtual int hitObject(Object* obj, Actor* actor, int inventoryIndex); //TODO
//This function is called when an object is going to be destroyed
//Use this function to do any cleanup if needed. The Object itself
//should NOT be deleted by this function
void ObjectProcessor::onDestroyObject(Object* obj)
{
    return;
}
#endif
