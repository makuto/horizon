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
//Used with system to create cell dirs
const std::string MAKE_DIR_COMMAND = "mkdir ";
const int NUM_LAYERS = 3;
const int ONGROUND_LAYER = 1;

bool CellIndexComparer::operator()(const CellIndex& first, const CellIndex& second) const
{
    if (first.x > second.x) return true;
    if (first.x < second.x) return false;
    return first.y > second.y;
}

Cell::Cell(CellIndex newCellID, World* newWorld, ObjectProcessorDir* processorDir):cellID(newCellID), objectManager(newWorld, processorDir, newCellID, this)
{
    world = newWorld;
    tiles.resize(NUM_LAYERS);
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
}
void Cell::init(CellIndex newCellID, World* newWorld, ObjectProcessorDir* processorDir)
{
    world = newWorld;
    cellID = newCellID;
    objectManager.init(newWorld, processorDir, newCellID, this);
}
Cell::~Cell()
{
    for (std::vector<std::vector<tile> >::iterator it = tiles.begin();
    it!=tiles.end(); ++it)
    {
        for (std::vector<tile>::iterator tIt = (*it).begin(); tIt != (*it).end(); ++tIt)
        {
            //delete (*tIt);
        }
    }
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
bool Cell::load(int worldID)
{
    std::ostringstream cellFileName;
    cellFileName << WORLDS_PATH << "world" << worldID <<"/cells/" << cellID.x << "-" << cellID.y << "/" << cellID.x << "-" << cellID.y << ".map";
    std::cout << "Attempting to load '" << cellFileName.str() << "'...\n";
    //Load master layer (.map)
    if (!loadLayer(cellFileName.str(), 0, true)) return false;
    //Load all .maplayers
    for (int i = 1; i < NUM_LAYERS; ++i)
    {
        std::ostringstream layerFileName;
        layerFileName << cellFileName.str() << "layer" << i;
        if (!loadLayer(layerFileName.str(), i, false)) return false;
        //std::cout << "Successfully loaded " << layerFileName.str() << "\n";
    }
    std::cout << "Successfully loaded " << cellFileName.str() << "\n";
    return true;
}
void Cell::generate(int worldID, int seed, int algorithm)
{
    //TODO: Make this settable elsewhere
    unsigned char defaultTileX = 4;
    unsigned char defaultTileY = 12;
    //I doubt there will be very many, so I'll just put it in switch
    switch (algorithm)
    {
        //Basic fill
        case 1:
            srand(seed);
            for (int i = 0; i < NUM_LAYERS; ++i)
            {
                std::vector<tile> layer;
                layer.resize(CELL_WIDTH * CELL_HEIGHT);
                for (int n = 0; n < CELL_WIDTH * CELL_HEIGHT; ++n)
                {
                    tile newTile;
                    //Different values based on layer
                    switch(i)
                    {
                        case 0:
                            //Ground layer
                            newTile.x = defaultTileX;
                            newTile.y = defaultTileY;
                            break;
                        case 1:
                            //OnGround layer
                            switch(rand() % 1000)
                            {
                                case 15:
                                    newTile.x = 0;
                                    newTile.y = 8;
                                    break;
                                case 16:
                                    newTile.x = 1;
                                    newTile.y = 8;
                                    break;
                                case 17:
                                    newTile.x = 3;
                                    newTile.y = 8;
                                    break;
                                case 18:
                                    newTile.x = 4;
                                    newTile.y = 8;
                                    break;
                                case 19:
                                    newTile.x = 5;
                                    newTile.y = 8;
                                    break;
                                default:
                                    newTile.x = 255;
                                    newTile.y = 255;
                            }
                            break;
                        case 2:
                            newTile.x = 255;
                            newTile.y = 255;
                            break;
                        default:
                            newTile.x = 255;
                            newTile.y = 255;
                            break;
                    }
                    layer[n] = newTile;
                }
                std::cout << "  Layer size: " << layer.size() << "\n";
                tiles[i] = layer;
            }
            std::cout << "Done generating " << tiles.size() << " of " << NUM_LAYERS << " layers\n";
            break;
        //Simplex noise world
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
        //default:
          //  std::cout << algorithm << "is not a valid algorithm (Cell.generate())\n";
           // break;
    }
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
void Cell::renderObjects(float viewX, float viewY, window* win)
{
    objectManager.renderObjects(viewX, viewY, win);
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
    /*//Set new layers
    for (unsigned int i = 0; i < NUM_LAYERS; i++)
    {
        map->setLayer(i, &tiles[i]);
    }
    
    //Render ground and onground
    map->render(0, 1, cam.getX(), cam.getY(), win);
    //Render Objects
    renderObjects(viewX, viewY, win);
    //Render aboveGround
    map->render(2, 0, cam.getX(), cam.getY(), win);*/
    renderBottom(cam, viewX, viewY, map, win);
    renderMiddle(cam, viewX, viewY, map, win);
    renderTop(cam, viewX, viewY, map, win);
}
void Cell::update(Time* globalTime)
{
    if (objectManager.updateObjects(globalTime))
    {
        setTouched(*globalTime);
    }
    return;
}
#endif


