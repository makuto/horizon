#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "../world/coord.hpp"
/* --Object--
 * Other than tiles and graphical effects, there are no other things in
 * the world but objects. Actors themselves are objects. Objects are dynamic
 * entities in the world.
 *
 * Objects become dynamic through ObjectProcessors. Object.type tells what
 * ObjectProcessor should be used.
 * */
struct Object
{
    //TODO: Make this union?
    Object* _nextPoolObject; //Don't use this for anything
    //type and subType are used by ObjectManager pools when an obj is uninitialized
    int type;       //General type of object, e.g. pickup, actor, usable
    int subType;    //Specific object of type, e.g. food pickup, human-species, bed
    int id;         //Universal ID for this instance
    Coord position; //World location for this object
    float rotation; //Graphical rotation for this object
};
#endif
