#ifndef COORD_CPP
#define COORD_CPP
#include <iostream>
#include "coord.hpp"

const int CELL_WIDTH = 64;
const int CELL_HEIGHT = 64;
const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
//Note that you want this value to always be larger than screen size
//UPDATE: No longer required to be larger than screen
const int CELL_WIDTH_PIXELS = CELL_WIDTH * TILE_WIDTH;
const int CELL_HEIGHT_PIXELS = CELL_HEIGHT * TILE_HEIGHT;

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
float Coord::getRelativeCellX(CellIndex& index)
{
    return (CELL_WIDTH_PIXELS * (cell.x - index.x)) + x;
}
float Coord::getRelativeCellY(CellIndex& index)
{
    return (CELL_HEIGHT_PIXELS * (cell.y - index.y)) + y;
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
