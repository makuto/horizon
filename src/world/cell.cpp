#ifndef CELL_CPP
#define CELL_CPP
#include <iostream>
//For building filename paths
#include <sstream>
//Saving maps
#include <fstream>
//TODO: Remove system
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <base2.0/tileMap/tileCamera.hpp>
#include "world.hpp"
#include "cell.hpp"
#include "../object/objectManager.hpp"
#include "../utilities/simplexnoise.h"
#include <cmath>
//Used in saveTileChanges
#include <list>
//Used with system to create cell dirs
const std::string MAKE_DIR_COMMAND = "mkdir ";
const int NUM_LAYERS = 3;
const int ONGROUND_LAYER = 1;

Cell::Cell(CellIndex newCellID, World* newWorld, ObjectProcessorDir* processorDir, RenderQueue* newRenderQueue):cellID(newCellID), objectManager(newWorld, processorDir, newCellID, this)
{
    world = newWorld;
    tiles.resize(NUM_LAYERS);
    onGroundPercentage = 0;
    unwalkablePercentage = 0;
    renderQueue = newRenderQueue;
    generated = false;
    changed = false;
}
Cell::Cell()
{
    cellID.x = 0;
    cellID.y = 0;
    world = NULL;
    tiles.resize(NUM_LAYERS);
    for (std::vector<std::vector<tile> >::iterator it = tiles.begin();
    it != tiles.end(); ++it)
    {
        (*it).resize(CELL_WIDTH * CELL_HEIGHT);
    }
    onGroundPercentage = 0;
    unwalkablePercentage = 0;
    renderQueue = NULL;
    generated = false;
    changed = false;
}
void Cell::init(CellIndex newCellID, World* newWorld, ObjectProcessorDir* processorDir, RenderQueue* newRenderQueue)
{
    world = newWorld;
    cellID = newCellID;
    objectManager.init(newWorld, processorDir, newCellID, this);
    touched.reset();
    onGroundPercentage = 0;
    unwalkablePercentage = 0;
    renderQueue = newRenderQueue;
    generated = false;
    changed = false;
}

void Cell::onDestroy()
{
    //Currently I don't do anything here
}

Cell::~Cell()
{
}
void Cell::calculateDifficulty()
{
    //Loop layers
    for (int i = 0; i < NUM_LAYERS; i++)
    {
        int totalTileCount = 0;
        int applicableCount = 0;
        //Loop tiles in layer
        for (std::vector<tile>::iterator tIt = tiles[i].begin(); tIt != tiles[i].end(); ++tIt)
        {
            totalTileCount++;
            switch(i)
            {
                case 0: //Ground layer
                    //TODO: Add functions that say what tiles are? (e.g isWater())
                    //If the tile is a water tile, add it to the count
                    if ((*tIt).y==0 && (*tIt).x <= 141)
                    {
                        applicableCount++;
                    }
                    break;
                case 1: //Onground layer
                    //If the tile isn't a null tile, add it to the count
                    if ((*tIt).x!=255 || (*tIt).y!=255)
                    {
                        applicableCount++;
                    }
                    break;
                default: //Everything else
                    break;
            }
        }
        //Calculate the percentages
        switch(i)
        {
            case 0: //Ground layer
                unwalkablePercentage = (float)applicableCount / (float)totalTileCount;
                //std::cout << applicableCount << " of " << totalTileCount << " ground tiles\n";
                break;
            case 1: //Onground layer
                //If the tile isn't a null tile, add it to the count
                onGroundPercentage = (float)applicableCount / (float)totalTileCount;
                //std::cout << applicableCount << " of " << totalTileCount << " onground tiles\n";
                break;
            default: //Everything else
                break; 
        }
    }
    //std::cout << "unwalkable: " << unwalkablePercentage << " onground: " << onGroundPercentage << "\n";
}
void Cell::setTouched(Time newValue)
{
    touched = newValue;
}
Time Cell::getTouched()
{
    return touched;
}
ObjectManager* Cell::getObjectManager()
{
    return &objectManager;
}
CellIndex Cell::getCellID()
{
    return cellID;
}
//Returns the value of changed, which is set to true of there are changes
//that should be saved
bool Cell::hasChanges()
{
    return changed;
}
//Returns the value of generated, which is set to true if this Cell
//was generated (if false, the Cell was probably loaded from file)
bool Cell::wasGenerated()
{
    return generated;
}
Cell* Cell::getNeighborCell(CellIndex index)
{
    return world->getCell(index);
}
CellIndex* Cell::getWorldIntersectingCells(Coord& topLeftCorner, float width, float height, int& size)
{
    return world->getIntersectingCells(topLeftCorner, width, height, size);
}
bool Cell::loadLayer(const std::string& filename, int layerNum, bool isMasterLayer)
{
    std::ifstream in;
	in.open(filename.c_str(), std::ios::binary);
	if (in.is_open())
	{
        char current;
        //Skip the header if it is a master layer
        if (isMasterLayer)
        {
            //Header is 9 chars
            for (int i = 0; i < 9; ++i)
            {
                in.get(current);
            }
        }

        //Get the binary tile data
        unsigned int currentIndex = 0;
		while (in.good() && !in.eof())
		{
			in.get(current);
			if (in.eof())
			{
				//delete newTile;
				break;
			}
            tile* currentTile = &tiles[layerNum][currentIndex];
			currentTile->x=(unsigned char) current;
			in.get(current);
			currentTile->y=(unsigned char) current;
            //TODO: Might be faster to resize first [UPDATE: Resize in constructor now]
            currentIndex++;
		}
	}
	else return false;
	in.close();
	return true;
}
bool Cell::load(const std::string& filename)
{
    std::cout << "Attempting to load '" << filename << "'... (LEGACY)\n";
    
    //Load master layer (.map)
    if (!loadLayer(filename, 0, true)) return false;
    //Load all .maplayers
    for (int i = 1; i < NUM_LAYERS; ++i)
    {
        std::ostringstream layerFileName;
        layerFileName << filename << "layer" << i;
        if (!loadLayer(layerFileName.str(), i, false)) return false;
    }
    std::cout << "Successfully loaded '" << filename << "'\n";
    calculateDifficulty();
    return true;
}
void Cell::generate(int worldID, int seed, int algorithm)
{
    //I doubt there will be very many, so I'll just put it in switch
    switch (algorithm)
    {
        //Simplex noise world; expects 0-255 colors (terrain.png); no biomes
        case 2:
            for (int y = 0; y < CELL_HEIGHT; ++y)
            {
                for (int x = 0; x < CELL_WIDTH; ++x)
                {
                    float noiseX = x + (CELL_WIDTH * cellID.x);
                    float noiseY = y + (CELL_HEIGHT * cellID.y);
                    noiseX /= 2; //TODO: Raise this value for more accurate floats far away?
                    noiseY /= 2;
                    const float SCALE = 0.001;
                    //TODO: Put all these values in a text file
                    float value = scaled_octave_noise_3d(10, 0.55, SCALE, 0, 255, noiseX, noiseY, seed);
                    //Winter bands
                    if (value > 142)
                    {
                        //value += 100 / (((int)(y+1) % 100) + 1);
                        //int yInt = (int) y;
                        float factor = fabs(sin(noiseY * SCALE));
                        const float SNOW_AMOUNT = 1.9;
                        factor -= SNOW_AMOUNT - factor; //1.3
                        if (factor < 0) factor = 0;
                        const float SNOW_FALLOFF = 1000;
                        value += SNOW_FALLOFF * factor; //Winter climates
                        if (value > 254) value = 254;
                        if (value < 142) value = 142;
                    }
                    //TEMP DELETE ME!
                    //value = 150;
                    if (value > 185) //Mountains are on multiple layers
                    {
                        //Ground and onground are solid black
                        tiles[0][x + (y * CELL_WIDTH)].x = 185;
                        tiles[0][x + (y * CELL_WIDTH)].y = 0;
                        tiles[1][x + (y * CELL_WIDTH)].x = 185;
                        tiles[1][x + (y * CELL_WIDTH)].y = 0;
                        //Above ground is elevation
                        tiles[2][x + (y * CELL_WIDTH)].x = value;
                        tiles[2][x + (y * CELL_WIDTH)].y = 0;
                    }
                    else
                    {
                        tiles[0][x + (y * CELL_WIDTH)].x = value;
                        tiles[0][x + (y * CELL_WIDTH)].y = 0;

                        tiles[1][x + (y * CELL_WIDTH)].x = 255;
                        tiles[1][x + (y * CELL_WIDTH)].y = 255;
                        tiles[2][x + (y * CELL_WIDTH)].x = 255;
                        tiles[2][x + (y * CELL_WIDTH)].y = 255;
                    }
                }
            }
            std::cout << "Done generating " << tiles.size() << " of " << NUM_LAYERS << " layers addr " << &tiles[0] << " \n";
            break;
        //Simplex noise (no biome); limited colors (3 for each range)
        case 3:
            for (int y = 0; y < CELL_HEIGHT; ++y)
            {
                for (int x = 0; x < CELL_WIDTH; ++x)
                {
                    float noiseX = x + (CELL_WIDTH * cellID.x);
                    float noiseY = y + (CELL_HEIGHT * cellID.y);
                    noiseX /= 2; //TODO: Raise this value for more accurate floats far away?
                    noiseY /= 2;
                    const float SCALE = 0.001;
                    //TODO: Put all these values in a text file
                    float value = scaled_octave_noise_3d(10, 0.55, SCALE, 0, 255, noiseX, noiseY, seed);
                    //Winter bands
                    if (value > 142)
                    {
                        //value += 100 / (((int)(y+1) % 100) + 1);
                        //int yInt = (int) y;
                        float factor = fabs(sin(noiseY * SCALE));
                        const float SNOW_AMOUNT = 1.9;
                        factor -= SNOW_AMOUNT - factor; //1.3
                        if (factor < 0) factor = 0;
                        const float SNOW_FALLOFF = 1000;
                        value += SNOW_FALLOFF * factor; //Winter climates
                        if (value > 254) value = 254;
                        if (value < 142) value = 142;
                    }
                    //TEMP DELETE ME!
                    //value = 150;
                    if (value > 185) //Mountains are on multiple layers
                    {
                        //Ground and onground are solid black
                        tiles[0][x + (y * CELL_WIDTH)].x = 9;
                        tiles[0][x + (y * CELL_WIDTH)].y = 0;
                        tiles[1][x + (y * CELL_WIDTH)].x = 9;
                        tiles[1][x + (y * CELL_WIDTH)].y = 0;
                        //Above ground is elevation (the 0.04... maps 0 - 70 to 0 - 3)
                        float limitedValue = (value - 185) * 0.042857143;
                        limitedValue += 9;
                        tiles[2][x + (y * CELL_WIDTH)].x = (unsigned char)limitedValue;
                        tiles[2][x + (y * CELL_WIDTH)].y = 0;
                    }
                    else
                    {
                        //The 0.04.... maps 0 - 185 to 0-8
                        float limitedValue = value * 0.043243243;
                        tiles[0][x + (y * CELL_WIDTH)].x = (unsigned char)limitedValue;
                        tiles[0][x + (y * CELL_WIDTH)].y = 0;
                        //Empty air for all other biomes
                        tiles[1][x + (y * CELL_WIDTH)].x = 255;
                        tiles[1][x + (y * CELL_WIDTH)].y = 255;
                        tiles[2][x + (y * CELL_WIDTH)].x = 255;
                        tiles[2][x + (y * CELL_WIDTH)].y = 255;
                    }
                }
            }
            std::cout << "Done generating " << tiles.size() << " of " << NUM_LAYERS << " layers addr " << &tiles[0] << " \n";
            break;
        default:
            std::cout << algorithm << " is not a valid algorithm (Cell.generate())\n";
            break;
    }
    calculateDifficulty();
    generated = true;
}
//Returns the tile at the x and y and layer
tile* Cell::getTileAt(int tileX, int tileY,  int layer)
{
    if (layer < NUM_LAYERS && tileX < CELL_WIDTH && tileY < CELL_HEIGHT)
    {
        return &tiles[layer][(tileY * CELL_WIDTH) + tileX];
    }
    else return NULL;
}
//Takes a value and converts it to the tile value (value % TILE_WIDTH
//or TILE_HEIGHT, depending on bool isX)
unsigned int Cell::pointToTileValue(float value, bool isX)
{
    if (isX) return value / TILE_WIDTH;
    return value / TILE_HEIGHT;
}

float Cell::getDifficulty()
{
    return (unwalkablePercentage + onGroundPercentage) / 2;
}
//Export a .maplayer (private and local b/c nothing else needs this)
bool exportAsLayer(const std::string& filename, std::vector<tile>* map, unsigned int layerIndex)
{
	std::ofstream out;
	std::ostringstream s;
	s << filename << "layer" << layerIndex;
	out.open(s.str().c_str(), std::ios::binary | std::ios::trunc);
	if (out.is_open())
	{
		for (std::vector<tile>::iterator it=map->begin(); it!=map->end(); ++it)
		{
			if (!out.good())
			{
				out.close();
				return false;
			};
			out << (*it).x;
			out << (*it).y;
		}
	}
	else return false;
	out.close();
	return true;
}
bool Cell::save(int worldID, dynamicMultilayerMap* map)
{
    //Get static map for exporting first layer
    dynamicTileMap *staticMap = map->getMasterMap();
    //Reset once we are done
    std::vector<tile>* previousMap = staticMap->getMap();

    //Create file system folders
    std::ostringstream fileStructureCommand;
    fileStructureCommand << MAKE_DIR_COMMAND << " " << WORLDS_PATH << "world" << worldID << "/cells";
    //TODO: remove system
    int result = system(fileStructureCommand.str().c_str());
    std::cout << fileStructureCommand.str() << " returned " << result << "\n";
    fileStructureCommand << "/" << cellID.x << "-" << cellID.y;
    result = system(fileStructureCommand.str().c_str());
    std::cout << fileStructureCommand.str() << " returned " << result << "\n";
    
    //Export first layer
    staticMap->setMap(&tiles[0]);
    std::ostringstream masterFileName;
    masterFileName << WORLDS_PATH << "world" << worldID <<"/cells/" <<
    cellID.x << "-" << cellID.y << "/" << cellID.x << "-" << cellID.y << ".map";
    //if (!staticMap->dumpMapToFile(masterFileName.str().c_str()))
    if (true)
    {
        std::cout << "Error dumping cell [ " << cellID.x << " , " <<
        cellID.y << " ] to " << masterFileName.str() << "\n";
        return false;
    }
    
    //Reset staticMap
    staticMap->setMap(previousMap);

    //Dump all other layers
    for (int i = 1; i < NUM_LAYERS; ++i)
    {
        //if (!exportAsLayer(masterFileName.str(), &tiles[i], i))
        if (true)
        {
            std::cout << "Error dumping cell [ " << cellID.x << " , " <<
            cellID.y << " ] layer " << i << " to " << masterFileName.str() << "\n";
            return false;
        }
    }
    std::cout << "Successfully dumped cell [ " << cellID.x << " , " <<
    cellID.y << " ] to " << masterFileName.str() << "\n";
    return true;
}
//Saves the map tiles using Run Length Encoding for compression.
//All layers are saved in a single file
bool Cell::saveTilesAsRLEmap(const std::string& filename)
{
    std::ofstream out;

    std::cout << "Attempting to save '" << filename << "'... (RLEM)\n";
    
	out.open(filename.c_str(), std::ios::binary | std::ios::trunc);
	if (out.is_open() && out.good())
	{
        //Output header information (size of map)
        out << (unsigned char)NUM_LAYERS << (unsigned char)CELL_WIDTH << (unsigned char)CELL_HEIGHT;
        
        unsigned char currentRunCount;
        tile* currentRunTile = NULL;
        //Loop through all layers
        for (std::vector<std::vector<tile> >::iterator lIt=tiles.begin(); lIt!=tiles.end(); ++lIt)
        {
            //Loop through each tile
            for (std::vector<tile>::iterator it=(*lIt).begin(); it!=(*lIt).end(); ++it)
            {
                tile* currentTile = &(*it);
                if (!currentRunTile) //First tile needs to be set as first run tile
                {
                    currentRunTile = currentTile;
                    currentRunCount = 1;
                    continue;
                }
                //Found a different tile or got max length of a run;
                //output the current run and start a new run
                if (currentTile->x != currentRunTile->x || currentTile->y != currentRunTile->y || currentRunCount == 255)
                {
                    if (!out.good())
                    {
                        out.close();
                        return false;
                    }
                    out << currentRunCount << currentRunTile->x << currentRunTile->y;
                    currentRunTile = currentTile;
                    currentRunCount = 1;
                }
                else
                    currentRunCount++; //Continue the run
            }
        }
    }
    else
    {
        std::cout << "ERROR: Cell::saveTilesAsRLEmap(): There was a problem opening '" << filename << "' for writing!\n";
        return false;
    }
    //Successfully saved the map!
    out.close();
    return true;
}
//Loads a map's tiles using Run Length Encoding for compression.
//All layers are saved in a single file
bool Cell::loadTilesFromRLEmap(const std::string& filename)
{
    std::ifstream in;

    std::cout << "Attempting to load '" << filename << "'... (RLEM)\n";
    int totalTiles = 0; //For debugging
    
    in.open(filename.c_str(), std::ios::binary);
    if (in.is_open())
    {
        //Get the map size first; if it isn't what is expected, break
        char currentChar;
        if (in.good() && !in.eof())
        {
            in.get(currentChar);
            unsigned char fileNumLayers = (unsigned char)currentChar;
            if (fileNumLayers != NUM_LAYERS)
            {
                std::cout << "ERROR: Cell::loadTilesFromRLEmap(): File '" << filename << "' contains " << (int) fileNumLayers << " layers but we expect " << NUM_LAYERS << "!\n";
                in.close();
                return false;
            }
            in.get(currentChar);
            unsigned char fileCellWidth = (unsigned char)currentChar;
            in.get(currentChar);
            unsigned char fileCellHeight = (unsigned char)currentChar;
            if (fileCellWidth != CELL_WIDTH || fileCellHeight != CELL_HEIGHT)
            {
                std::cout << "ERROR: Cell::loadTilesFromRLEmap(): File '" << filename << "' specifies its size as " << (int) fileCellWidth << "x" << (int) fileCellHeight << " but we expect " << CELL_WIDTH << "x" << CELL_HEIGHT << "!\n";
                in.close();
                return false;
            }
        }
        else
        {
            std::cout << "ERROR: Cell::loadTilesFromRLEmap(): There was a problem opening '" << filename << "' for reading!\n";
            in.close();
            return false;
        }

        //Begin reading the file
        int totalTilesThisLayer = 0;
        int currentLayerIndex = 0;
        const int TOTAL_TILES_PER_LAYER = CELL_WIDTH * CELL_HEIGHT;
        std::vector<tile>* currentLayer = &tiles[currentLayerIndex];
        if (!currentLayer)
        {
            std::cout << "ERROR: Cell::loadTilesFromRLEmap(): Failed to get layer " << currentLayerIndex << "!\n";
            in.close();
            return false;
        }
        while (in.good() && !in.eof())
        {
            //Get the current run
            in.get(currentChar);
            unsigned char runLength = (unsigned char) currentChar;
            tile currentTile;
            in.get(currentChar);
            currentTile.x = (unsigned char) currentChar;
            in.get(currentChar);
            currentTile.y = (unsigned char) currentChar;

            //Output the run to tiles
            for (unsigned char i = 0; i < runLength; i++)
            {
                (*currentLayer)[totalTilesThisLayer] = currentTile;
                totalTilesThisLayer++;
                totalTiles++;
                //If we have finished filling a whole layer, make sure
                //we move to the next layer
                if (totalTilesThisLayer == TOTAL_TILES_PER_LAYER)
                {
                    currentLayerIndex++;
                    if (currentLayerIndex >= NUM_LAYERS) break;
                    totalTilesThisLayer = 0;
                    currentLayer = &tiles[currentLayerIndex];
                    if (!currentLayer)
                    {
                        std::cout << "ERROR: Cell::loadTilesFromRLEmap(): Failed to get layer " << currentLayerIndex << "!\n";
                        in.close();
                        return false;
                    }
                }
            }
            if (currentLayerIndex >= NUM_LAYERS) break;
        }
    }
    else
    {
        std::cout << "ERROR: Cell::loadTilesFromRLEmap(): There was a problem opening '" << filename << "' for writing!\n";
        return false;
    }
    //Successfully loaded the tiles!
    std::cout << "Cell::loadTilesFromRLEmap(): Successfully loaded " << totalTiles << " tiles from '" << filename << "', expected " << NUM_LAYERS * CELL_WIDTH * CELL_HEIGHT << " tiles\n"; 
    in.close();
    return true;
}
void Cell::renderObjects(float viewX, float viewY, window* win)
{
    objectManager.renderObjects(viewX, viewY, win, renderQueue);
}
void Cell::renderBottom(tileCamera& cam, float viewX, float viewY, dynamicMultilayerMap* map, window* win)
{
    //Set new layers
    for (unsigned int i = 0; i < 2; i++)
    {
        map->setLayer(i, &tiles[i]);
    }
    //Render ground and onground
    map->render(0, 1, cam.getX(), cam.getY(), win);
}
void Cell::renderMiddle(tileCamera& cam, float viewX, float viewY, dynamicMultilayerMap* map, window* win)
{
    //Render Objects
    renderObjects(viewX, viewY, win);
}
void Cell::renderTop(tileCamera& cam, float viewX, float viewY, dynamicMultilayerMap* map, window* win)
{
    map->setLayer(2, &tiles[2]);
    //Render aboveGround
    map->render(2, 0, cam.getX(), cam.getY(), win);
}
void Cell::render(tileCamera& cam, float viewX, float viewY, dynamicMultilayerMap* map, window* win)
{
    renderBottom(cam, viewX, viewY, map, win);
    renderMiddle(cam, viewX, viewY, map, win);
    renderTop(cam, viewX, viewY, map, win);
}
void Cell::update(Time* globalTime)
{
    if (objectManager.updateObjects(globalTime)) //Keep cell alive if man had active objs
    {
        setTouched(*globalTime);
    }
    return;
}

//DELETEME
bool Cell::testRLESaveLoadFunctions(const std::string& filename)
{
    if (!saveTilesAsRLEmap(filename)) return false;
    std::cout << "  Save success\n";
    if (!loadTilesFromRLEmap(filename)) return false;
    std::cout << "  Load success\n";
    if (!loadTilesFromRLEmap("garbage.rlem"))
        std::cout << "  Load failure was a success!\n";
    /*if (!saveTilesAsRLEmap(filename)) return false;
    std::cout << "  Save 2 success\n";
    if (!loadTilesFromRLEmap(filename)) return false;
    std::cout << "  Load 2 success\n";*/
    return true;
}
#endif


