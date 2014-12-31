#ifndef OBJECT_CPP
#define OBJECT_CPP
#include "object.hpp"
#include "objectManager.hpp"
void Object::setPosition(Coord& newPosition, ObjectManager& manager)
{
    manager.moveObject(this, newPosition);
    bounds.setPosition(position.getCellOffsetX() + boundOffsetX, position.getCellOffsetY() + boundOffsetY);
}
void Object::addVector(float dx, float dy, ObjectManager& manager)
{
    Coord newPosition = position;
    newPosition.addVector(dx, dy);
    manager.moveObject(this, newPosition);
    bounds.setPosition(position.getCellOffsetX() + boundOffsetX, position.getCellOffsetY() + boundOffsetY);
}
Coord Object::getPosition()
{
    return position;
}
#endif
