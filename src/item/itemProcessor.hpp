#ifndef ITEMPROCESSOR_HPP
#define ITEMPROCESSOR_HPP
#include <base2.0/ept/eptParser.hpp>
#include "../object/object.hpp"
#include "item.hpp"
/* --ItemProcessor--
 * ItemProcessors provide the logic for a type of item. For example, a
 * sword would have a "WeaponItemProcessor." It's 'sword' distinction would
 * be a subtype of weapon (this would be dealt with by the processor).
 * Override these functions to create new item types.
 *
 * This is a very general and extremely powerful class - it should be able
 * to handle all items needed in the game
 *
 * Items are bound to Objects by default (they are designed to be used by
 * Agents, so this seemed OK), but there is no reason why you couldn't pass
 * NULL values for Objects when not needed - it's up to you.
 * */
class ObjectManager;
struct tile;
class World;
class window;
struct Need;
class ItemProcessor
{
    public:
        ItemProcessor();
        virtual ~ItemProcessor();

        //Use this function to create your itemProcessor. You must call
        //this function yourself. This encourages data-driven processors
        virtual bool initialize(eptFile* spec);
        
        /*Override these functions to create new item types. You don't need
         * to override functions that don't apply - they aren't pure virtual.
         * Return -1 if the item should be destroyed (which you will have to
         * handle; there is no way to know the context). All other return codes
         * are up to you. One very good use for return codes is to return
         * whether or not the method should be called again (e.g., a bow
         * item needs to be drawn before firing, so you would return 2 (or
         * any other number) and the caller would continue using the item
         * until you returned 1 (and the bow fires).*/
        //Use the item on the Object using it (e.g., use a health potion)
        virtual int useItemOnSelf(Item* item, Object* obj);
        //Use the item on another Object (e.g., use a sword on another agent)
        virtual int useItemOnObject(Item* item, Object* obj, Object* target);
        //Use the item on a tile (e.g., use a pickaxe on a stone)
        virtual int useItemOnTile(Item* item, Object* obj, tile* target);
        //Use the item on the world; this could do anything you can do through
        //World (spawn/locate Objects, destroy Objects, etc. An example of this
        //would be a staff that spawns goblins)
        virtual int useItemOnWorld(Item* item, Object* obj, World* world);
        //Use the item on a need; for example, use a food item to restore hunger
        virtual int useItemOnNeed(Item* item, Object* obj, Need* need);
        //Render the item at the specified position. Use useState to determine
        //how it should be rendered.
        virtual void renderItem(Item* item, Coord& position, window* win);
};
#endif
