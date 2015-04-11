#ifndef CONSUMABLE_ITEMPROCESSOR_HPP
#define CONSUMABLE_ITEMPROCESSOR_HPP
#include <base2.0/ept/eptParser.hpp>
#include "../itemProcessor.hpp"
#include "../../object/object.hpp"
#include "../item.hpp"

#include <base2.0/graphics/graphics.hpp>
/* --ConsumableItemProcessor--
 * Consumable items can be eaten/consumed to restore/harm Need levels.
 *
 * Consumables should be defined in the itemDatabase. Define the following
 * attributes:
 * bool stackable   Whether or not the item can be stacked
 * {int} needBenefits   Which needs are changed by consuming this item.
 *                      Use this format: {needID, change, needID, change...}
 *                      For example, to raise hunger (needID 2) by 50,
 *                      needBenefits={2, 50};
 * int difficulty   Used to determine how difficult it is to consume this item
 * float useTimeSeconds When consuming an item, it will take useTimeSeconds to finish
 *
 * Consumables use item.state when consuming an item to store how much longer
 * until the item is consumed
 * */
 
class ObjectManager;
struct tile;
class World;
class window;
struct Need;
struct Agent;
class ConsumableItemProcessor:public ItemProcessor
{
    private:
        sprite* itemSprite;
    public:
        ConsumableItemProcessor();
        virtual ~ConsumableItemProcessor();

        //Setup for consumable item processor
        void setup(sprite* renderSprite); //TEST

        //Use this function to create your itemProcessor. You must call
        //this function yourself. This encourages data-driven processors
        virtual bool initialize(eptFile* spec, ItemDatabase* newItemDB);
        
        /*Override these functions to create new item types. You don't need
         * to override functions that don't apply - they aren't pure virtual.
         * Return -1 if the item should be destroyed (which you will have to
         * handle; there is no way to know the context). All other return codes
         * are up to you. One very good use for return codes is to return
         * whether or not the method should be called again (e.g., a bow
         * item needs to be drawn before firing, so you would return 2 (or
         * any other number) and the caller would continue using the item
         * until you returned 1 (and the bow fires).*/
         //Use the item on the Object using it
        virtual int useItemOnSelf(Item* item, Object* obj, Time* deltaTime);
        //Use the item on another Object (e.g., use a sword on another agent)
        virtual int useItemOnObject(Item* item, Object* obj, Object* target, Time* deltaTime);
        //Use the item on a tile (e.g., use a pickaxe on a stone)
        virtual int useItemOnTile(Item* item, Object* obj, tile* target, Time* deltaTime);
        //Use the item on the world; this could do anything you can do through
        //World (spawn/locate Objects, destroy Objects, etc. An example of this
        //would be a staff that spawns goblins)
        virtual int useItemOnWorld(Item* item, Object* obj, World* world, Time* deltaTime);
        //Use the item on an Agent; for example, use a food item to restore hunger
        //Returns 0 while consuming and 1 when done.
        virtual int useItemOnAgent(Item* item, Object* obj, Agent* agent, Time* deltaTime);
        //Render the item at the specified position. Use useState to determine
        //how it should be rendered.
        virtual void renderItem(Item* item, Coord& position, float viewX, float viewY, float rotation, window* win);
};
#endif
