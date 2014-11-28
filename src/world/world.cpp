#ifndef WORLD_CPP
#define WORLD_CPP
#include <iostream>
#include "world.hpp"

//Where world should search for itself and its files
const std::string WORLDS_PATH = "worlds/";

World::World(window* newWin, multilayerMap* newMasterMap, int newWorldID)
{
    win = newWin;
    masterMap = newMasterMap;
    camera.setMap(masterMap->getMasterMap());
    camera.setViewSize(win->getWidth(), win->getHeight());
    //TODO
    worldID = newWorldID;
}
World::~World()
{
    //TODO: Delete all cells
}
bool World::loadCell(CellIndex cellToLoad)
{
    Cell* newCell = new Cell(cellToLoad, this);
    if (!newCell->load(worldID))
    {
        delete newCell;
        return false;
    }
    cells[cellToLoad] = newCell;
    return true;
}
Cell* World::getCell(CellIndex cell)
{
    //Find the cell
    std::map<CellIndex, Cell*, CellIndexComparer>::iterator findIt =
    cells.find(cell);
    if (findIt != cells.end())
    {
        //Return the found cell
        return findIt->second;
    }
    //Cell wasn't found!
    std::cout << "Cell [ " << cell.x << " , " << cell.y << " ] not found!\n";
    return NULL;
}
CellIndex* World::getIntersectingCells(Coord& topLeftCorner, float width, float height, int& size)
{
    CellIndex* cellArray = NULL;
    CellIndex topLeftCellIndex = topLeftCorner.getCell();
    Coord bottomR = topLeftCorner;
    bottomR.addVector(width, height);
    CellIndex bottomRCellIndex = bottomR.getCell();
    //The bottom corner is in a different cell
    if (bottomRCellIndex.x != topLeftCellIndex.x)
    {
        if (bottomRCellIndex.y != topLeftCellIndex.y)
        {
            cellArray = new CellIndex[4];
            size = 4;
            cellArray[0].x = topLeftCellIndex.x; //Top left
            cellArray[0].y = topLeftCellIndex.y;
            cellArray[1].x = bottomRCellIndex.x; //Bottom right
            cellArray[1].y = bottomRCellIndex.y;
            cellArray[2].x = bottomRCellIndex.x; //Top right
            cellArray[2].y = topLeftCellIndex.y;
            cellArray[3].x = topLeftCellIndex.x; //Bottom left
            cellArray[3].y = bottomRCellIndex.y;
        }
        else
        {
            cellArray = new CellIndex[2];
            size = 2;
            cellArray[0].x = topLeftCellIndex.x; //Top left
            cellArray[0].y = topLeftCellIndex.y;
            cellArray[1].x = bottomRCellIndex.x; //Top right
            cellArray[1].y = topLeftCellIndex.y;
        }
    }
    //The bottom corner is in a different Y cell
    else if (bottomRCellIndex.y != topLeftCellIndex.y)
    {
        cellArray = new CellIndex[2];
        size = 2;
        cellArray[0].x = topLeftCellIndex.x; //Top left
        cellArray[0].y = topLeftCellIndex.y;
        cellArray[1].x = topLeftCellIndex.x; //Bottom left
        cellArray[1].y = bottomRCellIndex.y;
    }
    //Only the current cell is visible
    else
    {
        cellArray = new CellIndex[1];
        size = 1;
        cellArray[0].x = topLeftCellIndex.x; //Top left
        cellArray[0].y = topLeftCellIndex.y;
    }
    return cellArray;
}
void World::render(Coord& viewPosition)
{
    //Set the camera to the view relative to the cell it's in
    int viewX = viewPosition.getCellOffsetX();
    int viewY = viewPosition.getCellOffsetY();
    int bottomLCornerX = CELL_WIDTH_PIXELS - viewX;
    int bottomLCornerY = CELL_HEIGHT_PIXELS - viewY;
    CellIndex viewPosCell = viewPosition.getCell();
    camera.setPosition(viewX, viewY);
    
    //Get an array of all cells the camera will see
    int size = 0;
    CellIndex* cellsToRender = getIntersectingCells(viewPosition, win->getWidth(), win->getHeight(), size);

    for (int i = 0; i < size; ++i)
    {
        //Set camera for other cells
        float newX = viewX;
        float newY = viewY;
        if (viewPosCell.x != cellsToRender[i].x)
        {
            newX = -bottomLCornerX;
        }
        if (viewPosCell.y != cellsToRender[i].y)
        {
            newY = -bottomLCornerY;
        }
        camera.setPosition(newX, newY);
        Cell* currentCell = getCell(cellsToRender[i]);
        if (currentCell)
        {
            currentCell->render(camera, masterMap, win);
        }
        //Eventually this will be somewhere else
        else
        {
            Cell* newCell = new Cell(cellsToRender[i], this);
            newCell->generate(worldID, cellsToRender[i].x + cellsToRender[i].y + worldID, 1);
            cells[cellsToRender[i]] = newCell;
        }
    }
    
    delete[] cellsToRender;
}
#endif

