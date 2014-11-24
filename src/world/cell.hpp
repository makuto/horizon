#ifndef CELL_HPP
#define CELL_HPP
#include <base2.0/tileMap/tileMap.hpp>
#include <base2.0/tileMap/multilayerMap.hpp>
#include <base2.0/tileMap/tileCamera.hpp>

extern const int NUM_LAYERS;
extern const int CELL_WIDTH_PIXELS;
extern const int CELL_HEIGHT_PIXELS;
//Simple x,y index to a cell
struct CellIndex
{
    int x;
    int y;
};
struct CellIndexComparer
{
    bool operator()(const CellIndex& first, const CellIndex& second) const;
};
/* --Cell--
 * Cell holds all data for a single cell/chunk of the world.
 * */
class Cell
{
    private:
        CellIndex cellID;
        //TODO: Consider putting layer information together (like RGB)
        //instead of in separate arrays (faster)
        std::vector<std::vector<tile*> > tiles;
        //Pass by value because string streams are used anyways
        bool loadLayer(const std::string& filename, int layerNum, bool isMasterLayer);
    public:
        Cell(CellIndex cellID);
        ~Cell();
        //Loads everything a Cell needs. Returns false if
        //any cell files were not found
        bool load(int worldID);
        //Saves everything a Cell has
        bool save(int worldID, multilayerMap* map);
        //Cam should be relative to the cell (use getCellOffsetX instead
        //of getTrueX)
        void render(tileCamera& cam, multilayerMap* map, window* win);
};
#endif



