#ifndef CELL_CPP
#define CELL_CPP
#include <iostream>
//For building filename paths
#include <sstream>
#include <base2.0/tileMap/tileCamera.hpp>
#include "world.hpp"
#include "cell.hpp"

const int NUM_LAYERS = 3;
extern const int CELL_WIDTH_PIXELS = 2048;
extern const int CELL_HEIGHT_PIXELS = 2048;

bool CellIndexComparer::operator()(const CellIndex& first, const CellIndex& second) const
{
    if (first.x > second.x) return true;
    if (first.x < second.x) return false;
    return first.y > second.y;
}

Cell::Cell(CellIndex newCellID)
{   
    cellID = newCellID;
    tiles.resize(NUM_LAYERS);
}
Cell::~Cell()
{
    //TODO: Delete cell data
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
bool Cell::save(int worldID, multilayerMap* map)
{
    return false;
}
void Cell::render(tileCamera& cam, multilayerMap* map, window* win)
{
    //Grab all layers already in the map
    std::vector<std::vector<tile*>* > oldLayers;
    for (unsigned int i = 0; i < map->getTotalLayers(); i++)
    {
        oldLayers.push_back(map->getLayer(i));
    }

    //Set new layers
    for (unsigned int i = 0; i < NUM_LAYERS; i++)
    {
        map->setLayer(i, &tiles[i]);
    }
    
    //Render
    map->render(0, 2, cam.getX(), cam.getY(), win);
    
    //Reset the layers
    for (unsigned int i = 0; i < map->getTotalLayers(); i++)
    {
        map->setLayer(i, oldLayers[i]);
    }
}
#endif


