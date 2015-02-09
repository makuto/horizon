#ifndef AGENTPROCESSOR_HPP
#define AGENTPROCESSOR_HPP
#include "../objectProcessor.hpp"
#include <base2.0/ept/eptParser.hpp>
#include "../object.hpp"
#include "../../world/time.hpp"
#include "../../world/coord.hpp"
#include "../../agent/agent.hpp"
#include "../../agent/processDirectory.hpp"
#include "../../agent/species.hpp"

class AgentProcessor:public ObjectProcessor
{
    private:
        ProcessDirectory* processDir;
        Species* species;

        sprite testSpr;
    public:
        AgentProcessor(Species* newSpecies, ProcessDirectory* newProcessDir);
        virtual ~AgentProcessor();
        //Use this function to init your custom ObjectProcessor
        virtual bool initialize(eptFile* spec);
        //--Overload functions for object dynamic behavior
        //--Functions should return -1 if the object should be destroyed
        //Called to initialize a new-ed object - use this function to make sure
        //objects are valid subtypes in valid positions.
        //Return false if the object cannot be created
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
        //    collision functions)
        //1 - The ObjectManager should try to resolve the collision. if
        //    isMoving is false, this means the stationary object will stay still
        //    and the moving object will be changed
        //2 - The ObjectManager should destroy the object
        //3 - The ObjectManager should move the object to the colliding
        //    coordinate and move everything else in the way (obviously, this
        //    doesn't work with tiles). If isMoving is false, this means
        //    the stationary (nonmoving) object will be moved by the moving object
        //Agent collides with object
        //virtual int touchObject(Object* collider, Coord& collideDisplacement, Agent* agent, bool isMoving);
        //Object collides with tile
        virtual int onCollideTile(Object* collider, Coord& collideDisplacement, tile& touchedTile, bool isMoving);
        //Object collides with object
        virtual int onCollideObj(Object* collider, Coord& collideDisplacement, Object* obj, bool isMoving);
        
        //This function is called when an object is going to be destroyed
        //Use this function to do any cleanup if needed. The Object itself
        //should NOT be deleted by this function
        virtual void onDestroyObject(Object* obj);
};
#endif
