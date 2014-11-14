#ifndef COORD_CPP
#define COORD_CPP
#include "coord.hpp"
float Coord::getScreenX(Coord* viewPosition)
{
    return x - viewPosition->x;
}
float Coord::getScreenY(Coord* viewPosition)
{
    return y - viewPosition->y;
}
void Coord::addVector(float dX, float dY)
{
    x+=dX;
    y+=dY;
}
void Coord::setPosition(float newX, float newY)
{
    x = newX;
    y = newY;
}
#endif
