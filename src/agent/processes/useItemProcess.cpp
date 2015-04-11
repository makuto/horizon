#ifndef USEITEM_PROCESS_CPP
#define USEITEM_PROCESS_CPP
#include <iostream>
#include "useItemProcess.hpp"
#include "../agent.hpp"
#include "../needMath.hpp"

UseItemProcess::~UseItemProcess()
{
    
}
void UseItemProcess::setup(ItemDatabase* newItemDB, ItemManager* newItemManager)
{
    itemManager = newItemManager;
    itemDB = newItemDB;
}
int UseItemProcess::getDifficulty(Agent* agent, Need* need, int index)
{
    std::cout << "in difficulty (need " << need << " is " << (int)need->currentValue << ")\n";
    //If the index isn't 0, chances are the agent is going to harvest the
    //item first, then use it. We cannot know this, so UseItem will just
    //trust that they'll have the item by the time this is executed
    if (index != 0) return 15; //Ballpark difficulty figure
    int difficulty = -1;
    //Look for an item in the inventory that will benefit the agent's need
    for (int i = 0; i < AGENT_INVENTORY_SIZE; i++)
    {
        Item* currentItem = &agent->inventory[i];
        //Skip item if not consumable (indicated by 1)
        if (currentItem->type != 1) continue;
        //Find whether or not this item helps the current need
        std::string needBenefits = itemDB->getStringAttribute(currentItem->type, currentItem->subType, "needBenefits");
        int benefitsLength = attrToArrayLength(needBenefits);
        for (int n = 0; n < benefitsLength; n+=2)
        {
            int needID = attrToArrayInt(needBenefits, n);
            if (needID == need->needID)
            {
                if (attrToArrayInt(needBenefits, n + 1) > 0) //Does this benefit the need?
                {
                    int currentDifficulty = itemDB->getIntAttribute(currentItem->type, currentItem->subType, "difficulty");
                    if (currentDifficulty < difficulty || difficulty == -1) difficulty = currentDifficulty;
                }
            }
        }
    }
    std::cout << "in difficulty done\n";
    return difficulty;
}

//Update should return 0 if process didn't complete (run next frame)
//1 if process is ready to go to the next process in chain
//-1 if process chain should end immediately
int UseItemProcess::update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime)
{
    //No target item; find a beneficial one
    if (agent->targetID == -1)
    {
        //Look for an item in the inventory that will benefit the agent's need
        for (int i = 0; i < AGENT_INVENTORY_SIZE; i++)
        {
            Item* currentItem = &agent->inventory[i];
            //Skip item if not consumable (indicated by 1)
            if (currentItem->type != 1) continue;
            //Find whether or not this item helps the current need
            std::string needBenefits = itemDB->getStringAttribute(currentItem->type, currentItem->subType, "needBenefits");
            int benefitsLength = attrToArrayLength(needBenefits);
            for (int n = 0; n < benefitsLength; n+=2)
            {
                int needID = attrToArrayInt(needBenefits, n);
                if (needID == need->needID && attrToArrayInt(needBenefits, n + 1) > 0) //The need benefits 
                {
                    agent->targetID = i;
                    break;
                }
            }
            if (agent->targetID != -1) break;
        }
    }
    if (agent->targetID == -1)
    {
        std::cout << "ERROR: UseItemProcess: Target item is " << agent->targetID << "!\n";
        return -1;
    }
    Item* targetItem = &agent->inventory[agent->targetID];
    //Grab the itemProcessor for the target item
    ItemProcessor* itemProcessor = itemManager->getItemProcessor(targetItem->type);
    if (!itemProcessor)
    {
        std::cout << "ERROR: UseItemProcess: ItemProcessor for " << targetItem->type << " not found!\n";
        return -1;
    }
    //Use the item on the current agent
    int status = itemProcessor->useItemOnAgent(targetItem, obj, agent, deltaTime);
    if (status == 1)
    {
        std::cout << "Consumed item; need now at " << (int)need->currentValue << "\n";
        //Done, reset targetID
        agent->targetID = -1;
        return 1;
    }
    else return 0;
}
#endif
