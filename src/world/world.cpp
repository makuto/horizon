#ifndef WORLD_CPP
#define WORLD_CPP
#include <iostream>
#include <base2.0/timer/timer.hpp>
#include "world.hpp"

//Where world should search for itself and its files
const std::string WORLDS_PATH = "worlds/";
const unsigned int MAX_INTERSECTING_CELLS = 10;
const int UPDATE_CLOSE_DISTANCE_X = 2048;
const int UPDATE_CLOSE_DISTANCE_Y = 2048;
const float MAX_WORLD_FAR_UPDATE = 0.01;

World::World(window* newWin, multilayerMap* newMasterMap, int newWorldID, ObjectProcessorDir* newDir)
{
    win = newWin;
    masterMap = newMasterMap;
    //Copy masterMap original layers to temporary storage
    for (unsigned int i = 0; i < masterMap->getTotalLayers(); i++)
    {
        oldLayers.push_back(masterMap->getLayer(i));
    }
    camera.setMap(masterMap->getMasterMap());
    camera.setViewSize(win->getWidth(), win->getHeight());
    processorDir = newDir;
    //TODO
    worldID = newWorldID;
    cellArrayCache = new CellIndex[MAX_INTERSECTING_CELLS];
    nextCellToUpdate =  cells.begin();
}
World::~World()
{
    delete[] cellArrayCache;
    for (std::map<CellIndex, Cell*, CellIndexComparer>::iterator it = cells.begin();
    it != cells.end(); ++it)
    {
        delete it->second;
    }
    //Reset masterMap layers
    for (unsigned int i = 0; i < masterMap->getTotalLayers(); i++)
    {
        masterMap->setLayer(i, oldLayers[i]);
    }
}
Cell* World::loadCell(CellIndex cellToLoad)
{
    Cell* newCell = new Cell(cellToLoad, this, processorDir);
    if (!newCell->load(worldID))
    {
        delete newCell;
        return NULL;
    }
    cells[cellToLoad] = newCell;
    return newCell;
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
    //Cell wasn't found! See if it is on the hard drive
    Cell* newCell = loadCell(cell);
    if (!newCell) //Cell isn't on hard drive; generate it now
    {
        newCell = new Cell(cell, this, processorDir);
        newCell->generate(worldID, cell.x + cell.y + worldID, 1);
        cells[cell] = newCell;
    }
    return newCell;
}
CellIndex* World::getIntersectingCells(Coord& topLeftCorner, float width, float height, int& size)
{
    CellIndex* cellArray = NULL;
    CellIndex topLeftCellIndex = topLeftCorner.getCell();
    Coord bottomR = topLeftCorner;
    bottomR.addVector(width, height);
    CellIndex bottomRCellIndex = bottomR.getCell();
    int numCols = abs(topLeftCellIndex.x - bottomRCellIndex.x) + 1;
    int numRows = abs(topLeftCellIndex.y - bottomRCellIndex.y) + 1;
    //cellArray = new CellIndex[numCols * numRows];
    cellArray = cellArrayCache;
    size = numCols * numRows;
    if ((unsigned int) size > MAX_INTERSECTING_CELLS)
    {
        std::cout << "ERROR: Intersecting cache size too small (needed " << size << " cells)!\n";
        size = 0;
        return cellArray;
    }
    int p = 0;
    for (int i = topLeftCellIndex.y; i < topLeftCellIndex.y + numRows; ++i)
    {
        for (int n = topLeftCellIndex.x; n < topLeftCellIndex.x + numCols; ++n)
        {
            cellArray[p].x = n;
            cellArray[p].y = i;
            p++;
        }
    }
    return cellArray;
    /*//The bottom corner is in a different cell
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
    return cellArray;*/
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
    //Render bottom layer
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
            //[DONE] TODO: Separate so that all cell grounds & ongrounds render,
            //then all cell objects, then all cell abovegrounds.
            currentCell->renderBottom(camera, newX, newY, masterMap, win);
        }
        //[DONE] TODO: Move this generation code
        /*else
        {
            Cell* newCell = new Cell(cellsToRender[i], this, processorDir);
            newCell->generate(worldID, cellsToRender[i].x + cellsToRender[i].y + worldID, 1);
            cells[cellsToRender[i]] = newCell;
        }*/
    }
    //Render middle layer
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
            currentCell->renderMiddle(camera, newX, newY, masterMap, win);
        }
    }
    //Render top layer
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
            currentCell->renderTop(camera, newX, newY, masterMap, win);
        }
    }
}
void World::update(Coord viewPosition, Time* globalTime, float extraTime)
{
    //Update close cells
    //Make viewPosition the top left corner of the close cells
    viewPosition.addVector(-UPDATE_CLOSE_DISTANCE_X, -UPDATE_CLOSE_DISTANCE_Y);
    int size;
    CellIndex* closeCells = getIntersectingCells(viewPosition,
    UPDATE_CLOSE_DISTANCE_X + CELL_WIDTH_PIXELS,
    UPDATE_CLOSE_DISTANCE_Y + CELL_HEIGHT_PIXELS, size);
    for (int i = 0; i < size; ++i)
    {
        Cell* currentCell = getCell(closeCells[i]);
        if (currentCell)
        {
            currentCell->update(globalTime);
        }
    }
    //Update other cells
    timer currentTime;
    currentTime.start();
    if (nextCellToUpdate==cells.end())
    {
        nextCellToUpdate = cells.begin();
    }
    for (; nextCellToUpdate!=cells.end();
    ++nextCellToUpdate)
    {
        if (currentTime.getTime() >= extraTime) break;
        nextCellToUpdate->second->update(globalTime);
    }
}
#endif

