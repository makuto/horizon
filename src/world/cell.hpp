#ifndef CELL_HPP
#define CELL_HPP
#include <string>
#include <base2.0/tileMap/tileMap.hpp>
#include <base2.0/tileMap/multilayerMap.hpp>
#include <base2.0/tileMap/tileCamera.hpp>

extern const std::string MAKE_DIR_COMMAND;
extern const int NUM_LAYERS;
extern const int CELL_WIDTH;
extern const int CELL_HEIGHT;
extern const int TILE_WIDTH;
extern const int TILE_HEIGHT;
//Note that you want this value to always be larger than screen size
extern const int CELL_WIDTH_PIXELS;
extern const int CELL_HEIGHT_PIXELS;
//Simple x,y index to a cell
struct CellIndex
{
    int x;
    int y;
};
//Used to be able to use CellIndex as a key in a std::map. See 
//http://stackoverflow.com/questions/6973406/c-stl-map-container-with-class-key-and-class-value
struct CellIndexComparer
{
    bool operator()(const CellIndex& first, const CellIndex& second) const;
};
/* --Cell--
 * Cell holds all data for a single cell/chunk of the world.
 * Cells always assume the multilayerMap is set up correctly;
 * cells never load information about displaying
 * */
class Cell
{
    private:
        CellIndex cellID;
        //TODO: Consider putting layer information together (like RGB)
        //instead of in separate arrays (faster)
        std::vector<std::vector<tile*> > tiles;
        //If isMasterLayer is true, this function will skip over the master
        //layer header and load the raw tiles
        bool loadLayer(const std::string& filename, int layerNum, bool isMasterLayer);
    public:
        Cell(CellIndex cellID);
        ~Cell();
        //Loads everything a Cell needs. Returns false if
        //any cell files were not found
        bool load(int worldID);
        //Saves everything a Cell has
        //TODO: Once editTile functions are made, keep a changed bool
        //that will be checked so that freshly generated tiles are not
        //saved
        bool save(int worldID, multilayerMap* map);
        //Generates a new cell with the specified algorithm
        //Algorithms: 1 = simple fill
        void generate(int worldID, int seed, int algorithm);
        //Cam should be relative to the cell (use getCellOffsetX instead
        //of getTrueX)
        void render(tileCamera& cam, multilayerMap* map, window* win);
};
#endif


