#ifndef OBJECTPROCESSOR_HPP
#define OBJECTPROCESSOR_HPP
#include <base2.0/ept/eptParser.hpp>
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/input/input.hpp>
#include <base2.0/tileMap/tileMap.hpp>
#include "object.hpp"
//#include "objectManager.hpp"
//#include "../agent/agent.hpp"
/* --ObjectProcessor--
 * ObjectProcessors take Objects and perform actions on them. This class
 * is polymorphic so new object behaviors can be implemented easily.
 * */
class ObjectManager;
class ObjectProcessor
{
    protected:
        //The supertype/type of objects this processor handles
        int processorType;  //Set to -1 by default (error if left at -1)
        
        sprite testSpr;
        inputManager* in;
    public:
        ObjectProcessor();
        //Make your own setup functions for custom data
        void setup(inputManager* newIn);
        virtual ~ObjectProcessor();
        
        int getType();

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
        virtual void renderObject(Object* obj, float viewX, float viewY, window* win);
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
        //TODO: Should I pass the pointer to the tile or not?
        virtual int onCollideTile(Object* collider, Coord& collideDisplacement, tile* touchedTile);
        //Object collides with object
        virtual int onCollideObj(Object* collider, Coord& collideDisplacement, Object* obj, bool isMoving);
        
        //This function is called when an object is going to be destroyed
        //Use this function to do any cleanup if needed. The Object itself
        //should NOT be deleted by this function
        virtual void onDestroyObject(Object* obj);
};
#endif
