#ifndef ITEMMANAGER_CPP
#define ITEMMANAGER_CPP
#include "itemManager.hpp"
//Add a processor to the map as the provided type processor
void ItemManager::addItemProcessor(int itemType, ItemProcessor* newProcessor)
{
    processors[itemType] = newProcessor;
}
//Returns the ItemProcessor for given type, or NULL if nonexistant
ItemProcessor* ItemManager::getItemProcessor(int itemType)
{
    std::map<int, ItemProcessor*>::iterator findIt = processors.find(itemType);
    if (findIt != processors.end())
    {
        return findIt->second;
    }
    else return NULL;
}
#endif
