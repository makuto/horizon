#ifndef $T_ITEMPROCESSOR_HPP
#define $T_ITEMPROCESSOR_HPP
#include <base2.0/ept/eptParser.hpp>
#include "../itemProcessor.hpp"
#include "../../object/object.hpp"
#include "../item.hpp"

/*To use template:
 * Save As -> yourItemProcessor.hpp (and .cpp) (e.g. weaponItemProcessor.hpp, weaponItemProcessor.cpp)
 * In both .cpp and .hpp:
 *  Replace $T with CAPS name (e.g $T -> WEAPON)
 *  Replace $t with Proper name (e.g $t -> Weapon)
 *  Replace $l with lower name (e.g. $l -> weapon)
 * 
 *  In .cpp, change the include "CHANGEME.hpp" to the name of this .hpp
 * 
 * Cut the text below into the makefile
        --------------
        under itemProcessor:
        --------------
$(OBJ_DIR)/$lItemProcessor.o: src/item/processors/$lItemProcessor.hpp src/item/processors/$lItemProcessor.cpp
	$(FLAGS) src/item/processors/$lItemProcessor.hpp
	$(FLAGS) src/item/processors/$lItemProcessor.cpp
        ---------------------------
        To horizon (after itemProcessor):
        ---------------------------
$(OBJ_DIR)/$lItemProcessor.o
        -------------------------
        To link (after itemProcessor):
        -------------------------
$lItemProcessor.o

 *
 * 
 * DELETE THIS COMMENT*/

/* --$tItemProcessor--
 * TODO: Document this ItemProcessor
 * */
 
class ObjectManager;
struct tile;
class World;
class window;
struct Need;
struct Agent;
class $tItemProcessor:public ItemProcessor
{
    public:
        $tItemProcessor();
        virtual ~$tItemProcessor();

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
        virtual int useItemOnAgent(Item* item, Object* obj, Agent* agent, Time* deltaTime);
        //Render the item at the specified position. Use useState to determine
        //how it should be rendered.
        virtual void renderItem(Item* item, Coord& position, float viewX, float viewY, float rotation, window* win);
};
#endif
