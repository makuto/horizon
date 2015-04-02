#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP
#include <map>
#include "itemProcessor.hpp"
/* --ItemManager--
 * ItemManager holds ItemProcessors. When something wants to use an item,
 * it will request the processor from this manager, then go through the processor. 
 * */
class ItemManager
{
    private:
        std::map<int, ItemProcessor*> processors;
    public:
        //Add a processor to the map as the provided type processor
        void addItemProcessor(int itemType, ItemProcessor* newProcessor);
        //Returns the ItemProcessor for given type, or NULL if nonexistant
        ItemProcessor* getItemProcessor(int itemType);
};
#endif
