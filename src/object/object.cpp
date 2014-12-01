#ifndef OBJECT_CPP
#define OBJECT_CPP
#include "object.hpp"
#include "objectManager.hpp"
void Object::setPosition(Coord& newPosition, ObjectManager& manager)
{
    manager.moveObject(this, newPosition);
}
void Object::addVector(float dx, float dy, ObjectManager& manager)
{
    Coord newPosition = position;
    newPosition.addVector(dx, dy);
    manager.moveObject(this, newPosition);
}
Coord Object::getPosition()
{
    return position;
}
#endif
