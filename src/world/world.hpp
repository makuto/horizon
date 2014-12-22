#ifndef WORLD_HPP
#define WORLD_HPP
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/tileMap/multilayerMap.hpp>
#include <base2.0/tileMap/tileCamera.hpp>
#include "coord.hpp"
#include "cell.hpp"
/* --World--
 * World manages all cells which compose the world as well as the
 * object lists.
 * */
extern const std::string WORLDS_PATH;
//Cells not near to the view (UPDATE_CLOSE_DISTANCE) will be updated
//after close cells for MAX_WORLD_FAR_UPDATE before breaking
extern const float MAX_WORLD_FAR_UPDATE;
class World
{
    private:
        int worldID;
        std::map<CellIndex, Cell*, CellIndexComparer> cells;
        multilayerMap* masterMap;
        //Holds the original masterMap layers 
        std::vector<std::vector<tile*>* > oldLayers;
        window* win;
        ObjectProcessorDir* processorDir;
        tileCamera camera;
        //Cache for GetIntersectingCells; set size via MAX_INTERSECTING_CELLS
        CellIndex* cellArrayCache;
        //Next cell to update in World.update
        std::map<CellIndex, Cell*, CellIndexComparer>::iterator nextCellToUpdate;
    public:
        //newMasterMap should be set up correctly already
        World(window* newWin, multilayerMap* newMasterMap, int newWorldID, ObjectProcessorDir* newDir);
        ~World();
        //Returns NULL if cell is not in map
        Cell* getCell(CellIndex cell);
        //Returns an array of pointers to all unique cells the range contains
        //[UPDATE: Now using a cached array with size MAX_INTERSECTING_CELLS] 
        //There is no reliable ordering, so use indices if that is needed
        //Note that this returns a maximum of four cells; cells must be
        //larger than the view in order for this to display correctly
        //TODO: Find better way to pass array, [FIXED; can now return
        //MAX_INTERSECTING_CELLS] make sure returning only four is viable
        //for other non-rendering uses
        CellIndex* getIntersectingCells(Coord& topLeftCorner, float width, float height, int& size);
        //loadCell attempts to load the cell from the filesystem:
        //{WORLD_CELLS_PATH}/world{worldID}/cells/{cellToLoad.x}-{cellToLoad.y}/{cellToLoad.x}-{cellToLoad.y}.map
        //Ex worlds/world1/cells/1-1/1-1.map
        //Returns NULL if the cell cannot be loaded
        Cell* loadCell(CellIndex cellToLoad);
        
        //Displays the map
        void render(Coord& viewPosition);
        //Updates objects
        //Objects nearest to viewPosition will be updated in real time,
        //then cells will be updated until extraTime is reached or the end of
        //the cells map
        void update(Coord viewPosition, Time* globalTime, float extraTime);
};
#endif
