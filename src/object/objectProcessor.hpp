#ifndef OBJECTPROCESSOR_HPP
#define OBJECTPROCESSOR_HPP
#include <base2.0/ept/eptParser.hpp>
#include "object.hpp"
#include "../agent/agent.hpp"
/* --ObjectProcessor--
 * ObjectProcessors take Objects and perform actions on them. This class
 * is polymorphic so new object behaviors can be implemented easily.
 * */
class ObjectProcessor
{
    private:
        //The supertype/type of objects this processor handles
        int processorType;  //Set to -1 by default (error if left at -1)
    public:
        ObjectProcessor();
        virtual ~ObjectProcessor();
        
        int getType();

        //Use this function to init your custom ObjectProcessor
        void initialize(eptFile* spec);
        //--Overload functions for object dynamic behavior
        //--Functions should return -1 if the object should be destroyed
        //Called to initialize a new-ed object - use this function to make sure
        //objects are valid subtypes in valid positions.
        //Return false if the object cannot be created
        virtual bool createObject(Object* newObj, int subType, Coord& position, float rotation);
        //Do a routine update on the object
        virtual int updateObject(Object* obj);
        //Render the object (it is in view of player)
        virtual void renderObject(Object* obj);
        //Agent uses/activates object
        virtual int activateObject(Object* obj, Agent* agent);
        //Agent collides with object
        virtual int touchObject(Object* obj, Agent* agent);
        //Object is hit by something
        //virtual int hitObject(Object* obj, Actor* actor, int inventoryIndex); //TODO
        //This function is called when an object is going to be destroyed
        //Use this function to do any cleanup if needed. The Object itself
        //should NOT be deleted by this function
        virtual void onDestroyObject(Object* obj);
};
#endif