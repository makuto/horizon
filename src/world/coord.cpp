#ifndef COORD_CPP
#define COORD_CPP
#include <iostream>
#include "coord.hpp"
float Coord::getCellOffsetX()
{
    return x;
}
float Coord::getCellOffsetY()
{
    return y;
}
CellIndex Coord::getCell()
{
    return cell;
}
float Coord::getTrueX()
{
    return (cell.x * CELL_WIDTH_PIXELS) + x;
}
float Coord::getTrueY()
{
    return (cell.y * CELL_HEIGHT_PIXELS) + y;
}
float Coord::getScreenX(Coord* viewPosition)
{
    //TODO: Will this be a problem with overflows in big maps?
    int newX = (cell.x * CELL_WIDTH_PIXELS) + x;
    int newViewX = (viewPosition->cell.x * CELL_WIDTH_PIXELS) + viewPosition->x;
    return newX - newViewX;
}
float Coord::getScreenY(Coord* viewPosition)
{
    //TODO: Will this be a problem with overflows in big maps?
    int newY = (cell.y * CELL_HEIGHT_PIXELS) + y;
    int newViewY = (viewPosition->cell.y * CELL_HEIGHT_PIXELS) + viewPosition->y;
    return newY - newViewY;
}
void Coord::print()
{
    std::cout << "  [ " << cell.x << " , " << cell.y << " ] ( " << x << " , " << y << " )\n";
}
void Coord::addVector(float dX, float dY)
{
    x+=dX;
    y+=dY;
    //Converts whole coord into one
    //TODO: Will this be a problem with overflows in big maps?
    int newX = (cell.x * CELL_WIDTH_PIXELS) + x;
    int newY = (cell.y * CELL_HEIGHT_PIXELS) + y;
    cell.x = newX / (CELL_WIDTH_PIXELS);
    x = newX % (CELL_WIDTH_PIXELS);
    cell.y = newY / (CELL_HEIGHT_PIXELS);
    y = newY % (CELL_HEIGHT_PIXELS);
    if (x < 0)
    {
        cell.x-=1;
        x = CELL_WIDTH_PIXELS + x;
    }
    if (y < 0)
    {
        cell.y-=1;
        y = CELL_HEIGHT_PIXELS + y;
    }
}
void Coord::setPosition(CellIndex& newCell, float newX, float newY)
{
    cell = newCell;
    x = newX;
    y = newY;
}
#endif
