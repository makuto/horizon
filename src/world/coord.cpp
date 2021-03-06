#ifndef COORD_CPP
#define COORD_CPP
#include <iostream>
#include <cmath>
#include "coord.hpp"
#include "time.hpp"

const int CELL_WIDTH = 64;
const int CELL_HEIGHT = 64;
const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
//The radius used to tell if objects are colliding with tiles
const int TILE_MANHATTAN_RADIUS = 1000;
//Note that you want this value to always be larger than screen size
//UPDATE: No longer required to be larger than screen
const int CELL_WIDTH_PIXELS = CELL_WIDTH * TILE_WIDTH;
const int CELL_HEIGHT_PIXELS = CELL_HEIGHT * TILE_HEIGHT;

bool CellIndexComparer::operator()(const CellIndex& first, const CellIndex& second) const
{
    if (first.x > second.x) return true;
    if (first.x < second.x) return false;
    return first.y > second.y;
}
bool Coord::operator==(const Coord& a)
{
    if (a.x != x || a.y != y || a.cell.x != cell.x || a.cell.y != cell.y) return false;
    return true;
}
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
    float newX = (cell.x * CELL_WIDTH_PIXELS) + x;
    float newViewX = (viewPosition->cell.x * CELL_WIDTH_PIXELS) + viewPosition->x;
    return newX - newViewX;
}
float Coord::getScreenY(Coord* viewPosition)
{
    //TODO: Will this be a problem with overflows in big maps?
    float newY = (cell.y * CELL_HEIGHT_PIXELS) + y;
    float newViewY = (viewPosition->cell.y * CELL_HEIGHT_PIXELS) + viewPosition->y;
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
//Returns the distance between the two
float Coord::getDistanceTo(Coord& point)
{
    return sqrtf(getManhattanTo(point));
}
//Returns the manhattan distance between the two
float Coord::getManhattanTo(Coord& point)
{
    CellIndex pointIndex = point.getCell();
    float aX = getRelativeCellX(pointIndex);
    float aY = getRelativeCellY(pointIndex);
    float bX = point.getRelativeCellX(pointIndex);
    float bY = point.getRelativeCellY(pointIndex);
    float xDiff = aX - bX;
    float yDiff = aY - bY;
    return abs(xDiff) + abs(yDiff);
}
void Coord::print()
{
    std::cout << "  [ " << cell.x << " , " << cell.y << " ] ( " << x << " , " << y << " )\n";
}
void Coord::addVector(float dX, float dY)
{
    x+=dX;
    y+=dY;
    if (x > CELL_WIDTH_PIXELS)
    {
        int movedCells = truncf(x / CELL_WIDTH_PIXELS);
        cell.x += movedCells;
        x -= movedCells * CELL_WIDTH_PIXELS;
    }
    if (y > CELL_HEIGHT_PIXELS)
    {
        int movedCells = truncf(y / CELL_HEIGHT_PIXELS);
        cell.y += movedCells;
        y -= movedCells * CELL_HEIGHT_PIXELS;
    }
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
    //Converts whole coord into one
    //TODO: Will this be a problem with overflows in big maps?
    /*float newX = (cell.x * CELL_WIDTH_PIXELS) + x;
    float newY = (cell.y * CELL_HEIGHT_PIXELS) + y;
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
    }*/
}
//Moves towards the given point at the specified speed
void Coord::moveTowards(Coord& point, float speed, Time* deltaTime)
{
    //Get the relative difference between this Coord and the next
    CellIndex pointIndex = point.getCell();
    float aX = getRelativeCellX(pointIndex);
    float aY = getRelativeCellY(pointIndex);
    float bX = point.getRelativeCellX(pointIndex);
    float bY = point.getRelativeCellY(pointIndex);
    float xDiff = -aX + bX;
    float yDiff = -aY + bY;
    //Normalize vector
    float distance = sqrtf((xDiff * xDiff) + (yDiff * yDiff));
    if (distance > 0)
    {
        xDiff /= distance;
        yDiff /= distance;
    }
    //Multiply vector by speed and deltaTime
    xDiff *= speed * deltaTime->getExactSeconds();
    yDiff *= speed * deltaTime->getExactSeconds();
    //Add the vector
    addVector(xDiff, yDiff);
}
void Coord::setPosition(CellIndex& newCell, float newX, float newY)
{
    cell = newCell;
    x = newX;
    y = newY;
}
void Coord::setPosition(int newCellX, int newCellY, float newX, float newY)
{
    cell.x = newCellX;
    cell.y = newCellY;
    x = newX;
    y = newY;
}
#endif
