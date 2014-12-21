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


//Used with system to create cell dirs
const std::string MAKE_DIR_COMMAND = "mkdir ";
const int NUM_LAYERS = 3;

bool CellIndexComparer::operator()(const CellIndex& first, const CellIndex& second) const
{
    if (first.x > second.x) return true;
    if (first.x < second.x) return false;
    return first.y > second.y;
}

Cell::Cell(CellIndex newCellID, World* newWorld, ObjectProcessorDir* processorDir):objectManager(newWorld, processorDir, newCellID, this)
{
    world = newWorld;
    cellID = newCellID;
    tiles.resize(NUM_LAYERS);
}
Cell::~Cell()
{
    for (std::vector<std::vector<tile*> >::iterator it = tiles.begin();
    it!=tiles.end(); ++it)
    {
        for (std::vector<tile*>::iterator tIt = (*it).begin(); tIt != (*it).end(); ++tIt)
        {
            delete (*tIt);
        }
    }
}
ObjectManager* Cell::getObjectManager()
{
    return &objectManager;
}
Cell* Cell::getNeighborCell(CellIndex index)
{
    return world->getCell(index);
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
		tile* newTile;
		while (in.good() && !in.eof())
		{
			newTile=new tile;
			in.get(current);
			if (in.eof())
			{
				delete newTile;
				break;
			}
			newTile->x=(unsigned char) current;
			in.get(current);
			newTile->y=(unsigned char) current;
            //TODO: Might be faster to resize first
			tiles[layerNum].push_back(newTile);
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
        std::cout << "Successfully loaded " << layerFileName.str() << "\n";
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
                std::vector<tile*> layer;
                layer.resize(CELL_WIDTH * CELL_HEIGHT);
                for (int n = 0; n < CELL_WIDTH * CELL_HEIGHT; ++n)
                {
                    tile* newTile = new tile;
                    //Different values based on layer
                    switch(i)
                    {
                        case 0:
                            //Ground layer
                            newTile->x = defaultTileX;
                            newTile->y = defaultTileY;
                            break;
                        case 1:
                            //OnGround layer
                            switch(rand() % 1000)
                            {
                                case 15:
                                    newTile->x = 0;
                                    newTile->y = 8;
                                    break;
                                case 16:
                                    newTile->x = 1;
                                    newTile->y = 8;
                                    break;
                                case 17:
                                    newTile->x = 3;
                                    newTile->y = 8;
                                    break;
                                case 18:
                                    newTile->x = 4;
                                    newTile->y = 8;
                                    break;
                                case 19:
                                    newTile->x = 5;
                                    newTile->y = 8;
                                    break;
                                default:
                                    newTile->x = 255;
                                    newTile->y = 255;
                            }
                            break;
                        case 2:
                            newTile->x = 255;
                            newTile->y = 255;
                            break;
                        default:
                            newTile->x = 255;
                            newTile->y = 255;
                            break;
                    }
                    layer[n] = newTile;
                }
                std::cout << "  Layer size: " << layer.size() << "\n";
                tiles[i] = layer;
            }
            std::cout << "Done generating " << tiles.size() << " of " << NUM_LAYERS << " layers\n";
            break;
        default:
            std::cout << algorithm << "is not a valid algorithm (Cell.generate())\n";
            break;
    }
}
//Export a .maplayer (private and local b/c nothing else needs this)
bool exportAsLayer(const std::string& filename, std::vector<tile*>* map, unsigned int layerIndex)
{
	std::ofstream out;
	std::ostringstream s;
	s << filename << "layer" << layerIndex;
	out.open(s.str().c_str(), std::ios::binary | std::ios::trunc);
	if (out.is_open())
	{
		for (std::vector<tile*>::iterator it=map->begin(); it!=map->end(); ++it)
		{
			if (!out.good())
			{
				out.close();
				return false;
			};
			out << (*it)->x;
			out << (*it)->y;
		}
	}
	else return false;
	out.close();
	return true;
}
bool Cell::save(int worldID, multilayerMap* map)
{
    //Get static map for exporting first layer
    staticTileMap *staticMap = map->getMasterMap();
    //Reset once we are done
    std::vector<tile*>* previousMap = staticMap->getMap();

    //Create file system folders
    std::ostringstream fileStructureCommand;
    fileStructureCommand << MAKE_DIR_COMMAND << " " << WORLDS_PATH << "world" << worldID << "/cells";
    //TODO: remove system
    int result = 0;
    result = system(fileStructureCommand.str().c_str());
    std::cout << fileStructureCommand.str() << " returned " << result << "\n";
    fileStructureCommand << "/" << cellID.x << "-" << cellID.y;
    result = system(fileStructureCommand.str().c_str());
    std::cout << fileStructureCommand.str() << " returned " << result << "\n";
    
    //Export first layer
    staticMap->setMap(&tiles[0]);
    std::ostringstream masterFileName;
    masterFileName << WORLDS_PATH << "world" << worldID <<"/cells/" <<
    cellID.x << "-" << cellID.y << "/" << cellID.x << "-" << cellID.y << ".map";
    if (!staticMap->dumpMapToFile(masterFileName.str().c_str()))
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
        if (!exportAsLayer(masterFileName.str(), &tiles[i], i))
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
void Cell::renderBottom(tileCamera& cam, float viewX, float viewY, multilayerMap* map, window* win)
{
    //Set new layers
    for (unsigned int i = 0; i < 2; i++)
    {
        map->setLayer(i, &tiles[i]);
    }
    //Render ground and onground
    map->render(0, 1, cam.getX(), cam.getY(), win);
}
void Cell::renderMiddle(tileCamera& cam, float viewX, float viewY, multilayerMap* map, window* win)
{
    //Render Objects
    renderObjects(viewX, viewY, win);
}
void Cell::renderTop(tileCamera& cam, float viewX, float viewY, multilayerMap* map, window* win)
{
    map->setLayer(2, &tiles[2]);
    //Render aboveGround
    map->render(2, 0, cam.getX(), cam.getY(), win);
}
void Cell::render(tileCamera& cam, float viewX, float viewY, multilayerMap* map, window* win)
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
    objectManager.updateObjects(globalTime);
    return;
}
#endif


