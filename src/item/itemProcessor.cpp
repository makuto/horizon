#ifndef ITEMPROCESSOR_CPP
#define ITEMPROCESSOR_CPP
#include "itemProcessor.hpp"

ItemProcessor::ItemProcessor()
{
}
ItemProcessor::~ItemProcessor()
{
    
}

//Use this function to create your itemProcessor. You must call
//this function yourself. This encourages data-driven processors
bool ItemProcessor::initialize(eptFile* spec, ItemDatabase* newItemDB)
{
    itemDB = newItemDB;
    return true;
}
//Override these functions to create new item types.
//Return -1 if the item should be destroyed. All other return codes
//are up to you.
//Use the item on the Object using it (e.g., use a health potion)
int ItemProcessor::useItemOnSelf(Item* item, Object* obj, Time* deltaTime)
{
    return 1;
}
//Use the item on another Object (e.g., use a sword on another agent)
int ItemProcessor::useItemOnObject(Item* item, Object* obj, Object* target, Time* deltaTime)
{
    return 1;
}
//Use the item on a tile (e.g., use a pickaxe on a stone)
int ItemProcessor::useItemOnTile(Item* item, Object* obj, tile* target, Time* deltaTime)
{
    return 1;
}
//Use the item on the world; this could do anything you can do through
//World (spawn/locate Objects, destroy Objects, etc. An example of this
//would be a staff that spawns goblins)
int ItemProcessor::useItemOnWorld(Item* item, Object* obj, World* world, Time* deltaTime)
{
    return 1;
}
//Use the item on an Agent; for example, use a food item to restore hunger
int ItemProcessor::useItemOnAgent(Item* item, Object* obj, Agent* agent, Time* deltaTime)
{
    return 1;
}
//Render the item at the specified position. Use state to determine
//how it should be rendered.
void ItemProcessor::renderItem(Item* item, Coord& position, float viewX, float viewY, float rotation, window* win)
{
}
#endif
