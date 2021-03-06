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
    if (fabs(dx) == 0 && fabs(dy) == 0) return; //Ignore zero movement
    Coord newPosition = position;
    newPosition.addVector(dx, dy);
    setPosition(newPosition, manager);
}
//Moves towards the given point at the specified speed
void Object::moveTowards(Coord& point, float speed, Time* deltaTime, ObjectManager& manager)
{
    Coord newPosition = position;
    newPosition.moveTowards(point, speed, deltaTime);
    setPosition(newPosition, manager);
}
Coord Object::getPosition()
{
    return position;
}

#endif
