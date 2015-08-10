#ifndef COMPONENT_HPP
#define COMPONENT_HPP
#include <base2.0/ept/eptParser.hpp>
#include "../world/time.hpp"
/* --Component--
 * Components are used to allow modular and reusable logic to be applied
 * to various objects. Unlike Needs, Agents, and Objects, Components are
 * instances rather than data to be processed. This allows them to store
 * per-object (or whatever is holding them) data.
 * */
class Object;
class ObjectManager;
class window;
class RenderQueue;
class Component
{
    protected:
        int type;       //The type of this component. Make sure to set the
                        //type in your Component's initialize function
        Object* obj;    //The Object this Component is bound to
    public:
        //Returns a new instance of this Component. This is used to allow
        //creation of components of specific types without any knowledge of
        //what type is being created
        virtual Component* createComponent();
        //Initialize this component with the provided spec. Called each time
        //a new component is created. This should wipe the values for your
        //component and bind it to the new object. Note that initialize will
        //be called multiple times if the Component is pooled.
        virtual bool initialize(eptFile* spec, Object* newObject);
        //Do whatever you want here. Note that you will need the ObjectProcessor
        //to update its components in its update function - it isn't done by
        //another system. Use int to return things like whether the object
        //should be destroyed or not
        virtual int update(Time* globalTime, ObjectManager* objectManager);
        //Like update(), your ObjectProcessor must call this (if it needs it)
        virtual void render(Object* obj, float viewX, float viewY, window* win, RenderQueue* renderQueue);
        int getType();
};
#endif
