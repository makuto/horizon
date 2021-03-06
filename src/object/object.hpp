#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <base2.0/collision/collision.hpp>
#include "../world/coord.hpp"
#include "../world/time.hpp"
//#include "objectManager.hpp"
/* --Object--
 * Other than tiles and graphical effects, there are no other things in
 * the world but objects. Actors themselves are objects. Objects are dynamic
 * entities in the world.
 *
 * Objects become dynamic through ObjectProcessors. Object.type tells what
 * ObjectProcessor should be used.
 * */
class ObjectManager;
class Object
{
    friend class ObjectManager;
    protected:
        //TODO: Make this union?
        Object* _nextPoolObject; //Don't use this for anything
    private:
        //Protect position for quadtree and collision code
        Coord position; //World location for this object
    public:
        //type and subType are used by ObjectManager pools when an obj is uninitialized
        int type;       //General type of object, e.g. pickup, actor, usable
        int subType;    //Specific object of type, e.g. food pickup, human-species, bed
        int id;         //Universal ID for this instance
        Time lastUpdate;//Last time this object was updated (you have control over this!
                        //It's not changed automatically - updateObject should handle that)
        int state;      //Use this for whatever you want
        float velX;     //Use these to store velocity. You have control over
        float velY;     //these - use them in renderQueue to make extrapolation work
        float rotation; //Graphical rotation for this object
        aabb bounds;    //Bounds for all external collisions (x and y ignored)
        float boundOffsetX; //Offsets for bounds (added to position; for center, you would do negative)
        float boundOffsetY;
        float manhattanRadius;  //The radius of this object as a manhattan value (squared radius)
                                //Used to determine if objects are "colliding" (they may not actually)
        int target;     //Used for whatever you want; recommended for obj ids, path IDs, etc.
        //Use these functions to manipulate the object's position. 
        //These functions make sure quadtree and collisions are handled
        //(which is why ObjectManager is passed in)
        void setPosition(Coord& newPosition, ObjectManager& manager);
        void addVector(float dx, float dy, ObjectManager& manager);
        //Moves towards the given point at the specified speed
        void moveTowards(Coord& point, float speed, Time* deltaTime, ObjectManager& manager);
        Coord getPosition();
        
};
#endif
