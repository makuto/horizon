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
class World
{
    private:
        int worldID;
        std::map<CellIndex, Cell*, CellIndexComparer> cells;
        multilayerMap* masterMap;
        window* win;
        tileCamera camera;
    public:
        //newMasterMap should
        World(window* newWin, multilayerMap* newMasterMap, int newWorldID);
        ~World();
        //Returns NULL if cell is not in map
        Cell* getCell(CellIndex cell);
        //Returns an array of pointers to all unique cells the range contains
        //Make sure to delete[] the array when you are done
        //There is no reliable ordering, so use indices if that is needed
        //Note that this returns a maximum of four cells; cells must be
        //larger than the view in order for this to display correctly
        //TODO: Find better way to pass array, make sure returning only
        //four is viable for other non-rendering uses
        CellIndex* getIntersectingCells(Coord& topLeftCorner, float width, float height, int& size);
        //loadCell attempts to load the cell from the filesystem:
        //{WORLD_CELLS_PATH}/world{worldID}/cells/{cellToLoad.x}-{cellToLoad.y}/{cellToLoad.x}-{cellToLoad.y}.map
        //Ex worlds/world1/cells/1-1/1-1.map
        bool loadCell(CellIndex cellToLoad);
        
        //Displays the map
        void render(Coord& viewPosition);
};
#endif
