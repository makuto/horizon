#ifndef $T_OBJECTPROCESSOR_HPP
#define $T_OBJECTPROCESSOR_HPP
#include <base2.0/ept/eptParser.hpp>
#include <base2.0/graphics/graphics.hpp>

#include "../objectProcessor.hpp"
#include "../object.hpp"


/*To use template:
 * Save As -> yourObjectProcessor.hpp (and .cpp) (e.g. treeObjectProcessor.hpp, treeObjectProcessor.cpp)
 * In both .cpp and .hpp:
 *  Replace $T with CAPS name (e.g $T -> TREE)
 *  Replace $t with Proper name (e.g $t -> Tree)
 *  Replace $l with lower name (e.g. $l -> tree)
 * 
 *  In .cpp, change the include "CHANGEME.hpp" to the name of this .hpp
 * 
 * Cut the text below into the makefile
        --------------
        under objectProcessor:
        --------------
$(OBJ_DIR)/$lObjectProcessor.o: src/object/processors/$lObjectProcessor.hpp src/object/processors/$lObjectProcessor.cpp
	$(FLAGS) src/object/processors/$lObjectProcessor.hpp
	$(FLAGS) src/object/processors/$lObjectProcessor.cpp
        ---------------------------
        To horizon (after objectProcessor):
        ---------------------------
$(OBJ_DIR)/$lObjectProcessor.o
        -------------------------
        To link (after objectProcessor):
        -------------------------
$lObjectProcessor.o

 *
 * 
 * DELETE THIS COMMENT*/

/* --$tObjectProcessor--
 * TODO: Document this ObjectProcessor
 * */
class ObjectManager;
class $tObjectProcessor:public ObjectProcessor
{
    protected:
        
    public:
        $tObjectProcessor();
        virtual ~$tObjectProcessor();

        //Use this function to init your custom ObjectProcessor (get images,
        //set default object starting values (hint: hold a object in your
        //processor with the values, then copy them over), etc.)
        //You should set processorType here
        //(you must call this - it initializes the master processor, not individual objects)
        virtual bool initialize(eptFile* spec);
        //--Overload functions for object dynamic behavior
        //--Functions should return -1 if the object should be destroyed
        //Called to initialize a new-ed object - use this function to make sure
        //objects are valid subtypes in valid positions.
        //Return false if the object cannot be created
        //You should initialize all of the parameter values in newObj as well as
        //Fields like bounds
        virtual bool initObject(Object* newObj, int subType, Coord& position, float rotation, ObjectManager* manager);
        //Do a routine update on the object; set obj->lastUpdate to globalTime
        //once finished (used to get the delta)
        virtual int updateObject(Object* obj, Time* globalTime, ObjectManager* manager);
        //Render the object (it is in view). ViewX and Y are the window's top left corner coordinates
        //relative to the current cell.
        virtual void renderObject(Object* obj, float viewX, float viewY, window* win, RenderQueue* renderQueue);
        //Agent uses/activates object
        //virtual int activateObject(Object* obj, Agent* agent);
        //Object is hit by a used item (sword etc)
        //virtual int hitObject(Object* obj, Actor* actor, int inventoryIndex); //TODO

        //Collision functions take the collider, the coordinate that the collider
        //is trying to reach (but collides with something), and the object/agent/tile
        //that the object is colliding with. Additionally, isMoving is passed that
        //indicates if this object is moving into others if true, or if objects
        //are moving into this object if false (if false, collideDisplacement is the
        //second object's, NOT the collider's)
        //By returning different codes, objects can control how they want the collision resolved:
        //(note that isMoving true objects take precedence)
        //0 - The ObjectManager should not try to resolve the collision
        //    (the collision is assumed to be resolved in the object's own
        //    collision functions). Simply set collideDisplacement to the desired
        //    position. Note that ObjectManager will assume that you have done
        //    this correctly.
        //1 - The ObjectManager should try to resolve/prevent the collision. if
        //    isMoving is false, this means the stationary object will stay still
        //    and the moving object will be changed
        //-1 - The ObjectManager should destroy the object
        //Agent collides with object
        //virtual int touchObject(Object* collider, Coord& collideDisplacement, Agent* agent, bool isMoving);
        //Object collides with tile (same return codes used)
        virtual int onCollideTile(Object* collider, Coord& collideDisplacement, tile* touchedTile);
        //Object collides with object
        virtual int onCollideObj(Object* collider, Coord& collideDisplacement, Object* obj, bool isMoving);
        
        //This function is called when an object is going to be destroyed
        //Use this function to do any cleanup if needed. The Object itself
        //should NOT be deleted by this function
        virtual void onDestroyObject(Object* obj);
};
#endif
