#ifndef WORLD_CPP
#define WORLD_CPP
#include <iostream>
#include <sstream> //For building cell file paths
#include <base2.0/timer/timer.hpp>
#include "world.hpp"
#include "../utilities/debugText.hpp"
#include "../utilities/simplexnoise.h" //For estimateDifficulty

//Where world should search for itself and its files
const std::string WORLDS_PATH = "worlds/";
//Tells cell which world generation algorithm to use to make its tiles
const int WORLD_GEN_ALGORITHM = 3;
const unsigned int MAX_INTERSECTING_CELLS = 10;
const int UPDATE_CLOSE_DISTANCE_X = 2048;
const int UPDATE_CLOSE_DISTANCE_Y = 2048;
const float MAX_WORLD_FAR_UPDATE_TIME = 0.001;
const unsigned int CELL_POOL_SIZE = 100;
//Number of seconds a cell has been untouched before the cell is unloaded
//Note that values larger than SECONDS_IN_DAY will be ignored because
//cells older than 1 day will always be unloaded
//Also note that CELL_UNLOAD_DELAY is not the final value. The final value is:
//CELL_UNLOAD_DELAY * (1 - (active cells / CELL_POOL_SIZE))
//The more active cells there are, the less the delay is
const float CELL_UNLOAD_DELAY = 60;
//The number of tiles to skip when estimating cell difficulties
const int CELL_ESTIMATION_SKIP = 16;


World::World(window* newWin, dynamicMultilayerMap* newMasterMap, int newWorldID, ObjectProcessorDir* newDir, RenderQueue* newRenderQueue, CELL_UNLOAD_MODE newCellUnloadMode):cellPool(CELL_POOL_SIZE)
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
    worldCellArrayCache = new CellIndex[MAX_INTERSECTING_CELLS];
    nextCellToUpdate =  cells.begin();
    renderQueue = newRenderQueue;
    cellUnloadMode = newCellUnloadMode;
}
World::~World()
{
    delete[] cellArrayCache;
    delete[] worldCellArrayCache;
    /*for (std::map<CellIndex, PoolData<Cell>*, CellIndexComparer>::iterator it = cells.begin();
    it != cells.end(); ++it)
    {
        //delete it->second;
    }*/
    //Reset masterMap layers
    for (unsigned int i = 0; i < masterMap->getTotalLayers(); i++)
    {
        masterMap->setLayer(i, oldLayers[i]);
    }
}
//Creates a file path to a Cell file (whether or not it exists)
//Note that fileExtension is with the dot, e.g. ".mlep"
std::string World::getPathToCellFile(CellIndex cellID, const std::string& fileExtension, bool isLegacy)
{
    std::ostringstream cellFileName;
    if (isLegacy)
    {
        cellFileName << WORLDS_PATH << "world" << worldID <<"/cellsLegacy/"
        << cellID.x << "-" << cellID.y << "/" << cellID.x << "-" << cellID.y << fileExtension;
    }
    else
        cellFileName << WORLDS_PATH << "world" << worldID <<"/cells/" << cellID.x << "-" << cellID.y << fileExtension;
        
    return cellFileName.str();
}
Cell* World::loadCell(CellIndex cellToLoad)
{
    //Get a free Cell from the pool
    PoolData<Cell>* newCell = cellPool.getNewData();
    if (newCell==NULL)
    {
        std::cout << "ERROR: world.loadCell(): Pool returned null!\n";
        return NULL;
    }
    //Initialize Cell
    newCell->data.init(cellToLoad, this, processorDir, renderQueue);

    //Generate the cell's path via WORLDS_PATH
    std::string cellFileName = getPathToCellFile(cellToLoad, ".map", true);
    
    //Attempt to load the cell using the LEGACY load function
    if (!newCell->data.load(cellFileName))
    {
        //LEGACY load failed; try loading an RLEM map
        cellFileName = getPathToCellFile(cellToLoad, ".rlem", false);
        //Attempt to load the cell; if it fails, free the cell and return NULL
        if (!newCell->data.loadTilesFromRLEmap(cellFileName))
        {
            cellPool.removeData(newCell);
            return NULL;
        }
    }

    cells[cellToLoad] = newCell;
    return &newCell->data;
}
Cell* World::getCell(CellIndex cell)
{
    //Find the cell
    std::map<CellIndex, PoolData<Cell>*, CellIndexComparer>::iterator findIt =
    cells.find(cell);
    if (findIt != cells.end())
    {
        //Return the found cell
        return &findIt->second->data;
    }
    
    //Cell wasn't found! See if it is on the hard drive
    Cell* newCell = loadCell(cell);
    
    if (!newCell) //Cell isn't on hard drive; generate it now
    {
        //Need to create the cell
        PoolData<Cell>* newPoolCell = cellPool.getNewData();
        if (newPoolCell==NULL)
        {
            std::cout << "ERROR: world.getCell(): Pool returned null! No space to generate...\n";
            return NULL;
        }
        newCell = &newPoolCell->data;
        newCell->init(cell, this, processorDir, renderQueue);
        newCell->generate(worldID, worldID, WORLD_GEN_ALGORITHM); //Seed is simply worldID
        cells[cell] = newPoolCell;
    }
    return newCell;
}
Cell* World::getCellIfExists(CellIndex cell)
{
    //Find the cell
    std::map<CellIndex, PoolData<Cell>*, CellIndexComparer>::iterator findIt =
    cells.find(cell);
    if (findIt != cells.end())
    {
        //Return the found cell
        return &findIt->second->data;
    }
    //Cell doesn't exist
    return NULL;
}
//World needs a private cache for getIntersectingCells; otherwise,
//any call to getIntersectingCells could affect update!
CellIndex* World::privateGetIntersectingCells(Coord& topLeftCorner, float width, float height, int& size)
{
    CellIndex topLeftCellIndex = topLeftCorner.getCell();
    Coord bottomR = topLeftCorner;
    bottomR.addVector(width, height);
    CellIndex bottomRCellIndex = bottomR.getCell();
    int numCols = abs(topLeftCellIndex.x - bottomRCellIndex.x) + 1;
    int numRows = abs(topLeftCellIndex.y - bottomRCellIndex.y) + 1;
    //cellArray = new CellIndex[numCols * numRows];
    CellIndex* cellArray = worldCellArrayCache;
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
}
CellIndex* World::getIntersectingCells(Coord& topLeftCorner, float width, float height, int& size)
{
    CellIndex topLeftCellIndex = topLeftCorner.getCell();
    Coord bottomR = topLeftCorner;
    bottomR.addVector(width, height);
    CellIndex bottomRCellIndex = bottomR.getCell();
    int numCols = abs(topLeftCellIndex.x - bottomRCellIndex.x) + 1;
    int numRows = abs(topLeftCellIndex.y - bottomRCellIndex.y) + 1;
    //cellArray = new CellIndex[numCols * numRows];
    CellIndex* cellArray = cellArrayCache;
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
}
float World::estimateCellDifficulty(CellIndex& cellToEstimate)
{
    //Calculate a few tiles in the cell to estimate
    float totalTiles = 0;
    float numWaterTiles = 0;
    float numMountainTiles = 0;
    for (int y = 0; y < CELL_HEIGHT; y += CELL_ESTIMATION_SKIP)
    {
        for (int x = 0; x < CELL_WIDTH; x += CELL_ESTIMATION_SKIP)
        {
            totalTiles++;
            float noiseX = x + (CELL_WIDTH * cellToEstimate.x);
            float noiseY = y + (CELL_HEIGHT * cellToEstimate.y);
            noiseX /= 2; //TODO: Raise this value for more accurate floats far away?
            noiseY /= 2;
            const float SCALE = 0.001;
            //TODO: Put all these values in a text file
            float value = scaled_octave_noise_3d(10, 0.55, SCALE, 0, 255, noiseX, noiseY, worldID);
            //TODO: Make a function that says what water and mountain is
            if (value < 142) numWaterTiles++;
            else if (value > 185) numMountainTiles++;
        }
    }
    float difficulty = ((numWaterTiles / totalTiles) + (numMountainTiles / totalTiles)) / 2;
    /*float trueDifficulty = -1;
    Cell* requestedCell = getCell(cellToEstimate);
    if (requestedCell)
    {
        trueDifficulty = requestedCell->getDifficulty();
    }
    std::cout << "Estimated water: " << numWaterTiles << " Estimated ongrounds: " << numMountainTiles << " total tiles picked: " << totalTiles << "\n";
    std::cout << "Estimated difficulty: " << difficulty << " via alg: " << ((numWaterTiles / totalTiles) + (numMountainTiles / totalTiles))  << " true difficulty: " << trueDifficulty << "\n\n";*/
    return difficulty;
}
/*//Searches all active ObjectManagers for an object with the provided ID
//Returns NULL if the object wasn't found
Object* findObject(int targetId)
{
    return NULL;
}*/
void World::render(Coord& viewPosition, Time* globalTime, float extrapolateAmount)
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
    //Render renderQueue onGround layer
    renderQueue->renderLayerExtrapolate(viewPosition, RENDER_QUEUE_LAYER::ONGROUND, extrapolateAmount);
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
            //Set touched on cell (to keep in memory) (only needs to be done once)
            currentCell->setTouched(*globalTime);
        }
    }
    //Render renderQueue aboveGround layer
    renderQueue->renderLayerExtrapolate(viewPosition, RENDER_QUEUE_LAYER::ABOVEGROUND, extrapolateAmount);
    //Render overlay
    /*sprite dayNightSpr;
    if (dayNightSpr.load("data/images/day-night.png"))
    {
        masterMap->setImage(&dayNightSpr);
        std::vector<tile> dayNightData;
        dayNightData.resize(64 * 64);
        std::cout << "populating\n";
        for (int i = 0; i < 64 * 64; ++i)
        {
            dayNightData[i].x = (globalTime->getSeconds() / SECONDS_IN_DAY) * 32;
            dayNightData[i].y = 0;
        }
        std::cout << "done\n";
        masterMap->setLayer(0, &dayNightData);
        std::cout << "rendering\n";
        masterMap->render(0, 0, 0, 0, win);
        std::cout << "done\n";
        
    }*/
}
void World::onCellUnload(Cell* cellToUnload)
{
    //Let the cell know we are destroying it
    cellToUnload->onDestroy();

    //Prepare filename to RLEM file in case the Cell will be saved
    CellIndex cellToSaveIndex = cellToUnload->getCellID();
    std::string filename = getPathToCellFile(cellToSaveIndex, ".rlem", false);
    
    //Do whatever unload mode requested
    switch(cellUnloadMode)
    {
        //Save the Cell only if it's been modified & needs persistence
        case CELL_UNLOAD_MODE::SAVE_ON_CHANGES:
            if (cellToUnload->hasChanges())
                cellToUnload->saveTilesAsRLEmap(filename);
            break;
        //Save the Cell if it was generated and doesn't exist in file
        case CELL_UNLOAD_MODE::SAVE_ON_GENERATION:  
            if (cellToUnload->wasGenerated())
                cellToUnload->saveTilesAsRLEmap(filename); 
            break;
        //Save the Cell if it was generated and doesn't exist in file or has changes
        case CELL_UNLOAD_MODE::SAVE_ON_GENERATION_OR_CHANGES:  
            if (cellToUnload->wasGenerated() || cellToUnload->hasChanges())
                cellToUnload->saveTilesAsRLEmap(filename); 
            break;
        //Save the Cell even if it has no changes & wasn't newly generated
        case CELL_UNLOAD_MODE::FORCE_SAVE: 
            cellToUnload->saveTilesAsRLEmap(filename); 
            break;
        //Any other mode discards the changes (if any)
        default:
            break;
    }
}
void World::update(Coord viewPosition, CellIndex* requestedCells, int requestedSize, Time* globalTime, float extraTime)
{
    DebugText::addEntry("Pool usage (%): ", ((float)cellPool.getTotalActiveData() / (float)CELL_POOL_SIZE) * 100);
    DebugText::addEntry("Unload delay (seconds): ", (CELL_UNLOAD_DELAY * (1 - ((float)cellPool.getTotalActiveData() / (float)CELL_POOL_SIZE))));
    //Update close cells
    //Make viewPosition the top left corner of the close cells
    viewPosition.addVector(-UPDATE_CLOSE_DISTANCE_X, -UPDATE_CLOSE_DISTANCE_Y);
    int size;
    CellIndex* closeCells = privateGetIntersectingCells(viewPosition,
    UPDATE_CLOSE_DISTANCE_X + CELL_WIDTH_PIXELS,
    UPDATE_CLOSE_DISTANCE_Y + CELL_HEIGHT_PIXELS, size);
    for (int i = 0; i < size; ++i)
    {
        Cell* currentCell = getCell(closeCells[i]);
        if (currentCell)
        {
            currentCell->update(globalTime);
            //Nearby cells will be touched so that they won't be unloaded
            currentCell->setTouched(*globalTime);
        }
    }
    //Update requested cells (e.g. cells with active events)
    for (int i = 0; i < requestedSize; i++)
    {
        Cell* currentCell = getCellIfExists(requestedCells[i]);
        if (currentCell)
        {
            //Skip any cells we updated in the view loop (above)
            if (currentCell->getTouched().getExactSeconds()==globalTime->getExactSeconds())
                continue;
            currentCell->update(globalTime);
            currentCell->setTouched(*globalTime);
        }
    }
    //Update other cells until time runs out
    //TODO: [DONE] Delete cells that are inactive for a long time
    timer currentTime;
    currentTime.start();
    if (nextCellToUpdate==cells.end())
    {
        nextCellToUpdate = cells.begin();
    }
    for (; nextCellToUpdate!=cells.end();
    ++nextCellToUpdate)
    {
        //Make sure we don't get stuck updating cells and harm the frame rate
        if (currentTime.getTime() >= extraTime) break;
        //Skip any cells we updated in the view loop (above)
        if (nextCellToUpdate->second->data.getTouched().getExactSeconds()==globalTime->getExactSeconds())
            continue;

        //Update the cell
        nextCellToUpdate->second->data.update(globalTime);
        
        //Check if touched delta is large; if so, remove the cell
        Time delta;
        Time cellTouched = nextCellToUpdate->second->data.getTouched();
        globalTime->getDeltaTime(&cellTouched, delta);
        delta.invert();
        //Equation explained at top of file; unloads cells faster depending on
        //how full the cell pool is (if it's very full, it will unload cells with
        //very small deltas
        if (delta.getExactSeconds() > (CELL_UNLOAD_DELAY * (1 - ((float)cellPool.getTotalActiveData() / (float)CELL_POOL_SIZE))) || delta.getDays() > 0)
        {
            if (nextCellToUpdate->second->data.getTouched().getExactSeconds()==0) 
            {
                //Cell is brand new; set time to current globalTime
                nextCellToUpdate->second->data.setTouched(*globalTime);
                continue;
            }

            //std::cout << "removing cell " << nextCellToUpdate->first.x << " , " << nextCellToUpdate->first.y << ", delta: "; delta.print();
            //Cell is old; remove it to free up pool space
            onCellUnload(&nextCellToUpdate->second->data);
            cellPool.removeData(nextCellToUpdate->second);
            cells.erase(nextCellToUpdate);
        }
    }
}
#endif

