#ifndef CONSUMABLE_ITEMPROCESSOR_CPP
#define CONSUMABLE_ITEMPROCESSOR_CPP
#include "consumableItemProcessor.hpp"
#include <map>
#include "../../agent/needMath.hpp"
#include "../../agent/agent.hpp"
#include "../itemDatabase.hpp"

ConsumableItemProcessor::ConsumableItemProcessor()
{
}
ConsumableItemProcessor::~ConsumableItemProcessor()
{
    
}

void ConsumableItemProcessor::setup(sprite* renderSprite)
{
    itemSprite = renderSprite; //TESTING ONLY
}
//Use this function to create your itemProcessor. You must call
//this function yourself. This encourages data-driven processors
bool ConsumableItemProcessor::initialize(eptFile* spec, ItemDatabase* newItemDB)
{
    itemDB = newItemDB;
    return true;
}
//Override these functions to create new item types.
//Return -1 if the item should be destroyed. All other return codes
//are up to you.
//Use the item on the Object using it (e.g., use a health potion)
int ConsumableItemProcessor::useItemOnSelf(Item* item, Object* obj, Time* deltaTime)
{
    return 1;
}
//Use the item on another Object (e.g., use a sword on another agent)
int ConsumableItemProcessor::useItemOnObject(Item* item, Object* obj, Object* target, Time* deltaTime)
{
    return 1;
}
//Use the item on a tile (e.g., use a pickaxe on a stone)
int ConsumableItemProcessor::useItemOnTile(Item* item, Object* obj, tile* target, Time* deltaTime)
{
    return 1;
}
//Use the item on the world; this could do anything you can do through
//World (spawn/locate Objects, destroy Objects, etc. An example of this
//would be a staff that spawns goblins)
int ConsumableItemProcessor::useItemOnWorld(Item* item, Object* obj, World* world, Time* deltaTime)
{
    return 1;
}
//Use the item on an Agent; for example, use a food item to restore hunger
int ConsumableItemProcessor::useItemOnAgent(Item* item, Object* obj, Agent* agent, Time* deltaTime)
{
    float useTimeSeconds = itemDB->getFloatAttribute(item->type, item->subType, "useTimeSeconds");
    item->state += deltaTime->getExactSeconds();
    if (item->state >= useTimeSeconds) //Item is now consumed
    {
        std::string needBenefits = itemDB->getStringAttribute(item->type, item->subType, "needBenefits");
        //Apply benefits as defined by needBenefits
        int benefitsLength = attrToArrayLength(needBenefits);
        std::map<int, int> benefits;
        for (int i = 0; i < benefitsLength; i += 2)
        {
            //TODO: Add function that gets requested need type (in species?)?
            benefits[attrToArrayInt(needBenefits, i)] = attrToArrayInt(needBenefits, i + 1);
        }
        //Go through vital and nonvital needs, adding depending on needBenefits
        for (int i = 0; i < agent->numVitalNeeds; i++)
        {
            Need* currentNeed = &agent->vitalNeeds[i];
            std::map<int, int>::iterator findIt = benefits.find(currentNeed->needID);
            if (findIt != benefits.end())
            {
                if (findIt->second < 0) currentNeed->currentValue = subNoOverflow(currentNeed->currentValue, findIt->second);
                else currentNeed->currentValue = addNoOverflow(currentNeed->currentValue, findIt->second);
            }
        }
        for (int i = 0; i < agent->numNonvitalNeeds; i++)
        {
            Need* currentNeed = &agent->nonvitalNeeds[i];
            std::map<int, int>::iterator findIt = benefits.find(agent->nonvitalNeeds[i].needID);
            if (findIt != benefits.end())
            {
                if (findIt->second < 0) currentNeed->currentValue = subNoOverflow(currentNeed->currentValue, findIt->second);
                else currentNeed->currentValue = addNoOverflow(currentNeed->currentValue, findIt->second);
            }
        }
        std::cout << "Consumed\n";
        //Destroy the item after use
        item->state = 0;
        item->stackCount--;
        if (item->stackCount<=0) //Stack used up; replace with empty item
        {
            item->type = -1;
            item->subType = -1;
            item->stackCount = 0;
            item->id = -1;
            item->useState = 0;
        }
        return 1;
    }
    return 0;
}
//Render the item at the specified position. Use state to determine
//how it should be rendered.
void ConsumableItemProcessor::renderItem(Item* item, Coord& position, float viewX, float viewY, float rotation, window* win)
{
    itemSprite->setPosition(position.getCellOffsetX() - viewX, position.getCellOffsetY() - viewY);
    itemSprite->setRotation(rotation);
    win->draw(itemSprite);
}
#endif
