#ifndef CELL_HPP
#define CELL_HPP
#include <string>
#include <base2.0/tileMap/tileMap.hpp>
#include <base2.0/tileMap/multilayerMap.hpp>
#include <base2.0/tileMap/tileCamera.hpp>
#include "../object/objectManager.hpp"
#include "../object/objectProcessorDir.hpp"
#include "time.hpp"
#include "../utilities/renderQueue.hpp"
#include "../utilities/pool.hpp"

extern const std::string MAKE_DIR_COMMAND;
extern const int NUM_LAYERS;
extern const int ONGROUND_LAYER;

/* --Cell--
 * Cell holds all data for a single cell/chunk of the world.
 * Cells always assume the multilayerMap is set up correctly;
 * cells never load information about displaying
 * */
class ObjectManager;
class Cell
{
    private:
        CellIndex cellID;
        //ObjectManager that holds all objects in this cell
        ObjectManager objectManager;
        //TODO: Consider putting layer information together (like RGB)
        //instead of in separate arrays (faster)
        std::vector<std::vector<tile> > tiles;

        //Stores the percentage of the amount of the specified type of
        //tiles (useful for estimating the "difficulty" of the cell) for
        //pathfinding - cell with a large percentage of onGrounds or water is
        //harder to navigate than a nearly empty cell.
        float onGroundPercentage; //Tiles on ground (walls etc. contribute)
        float unwalkablePercentage;//Tiles of ground (water etc. contribute)
        //Calculates the above percentages
        void calculateDifficulty();

        World* world;
        //If a cell is inactive for long periods of time, it can be unloaded.
        //Touched stores the last time it was active
        Time touched;

        //RenderQueue for Object layer rendering
        RenderQueue* renderQueue;

        //Set to true if there are changes this Cell has on it that should
        //be saved (TODO: Remember to set this if changes are made)
        bool changed;
        //Set to true if this cell was generated procedurally (if it is
        //false, it was loaded from a file or got its data elsewhere)
        bool generated;
        
        //If isMasterLayer is true, this function will skip over the master
        //layer header and load the raw tiles
        bool loadLayer(const std::string& filename, int layerNum, bool isMasterLayer);

    public:
        void setTouched(Time newValue);
        Time getTouched();
        
        Cell(CellIndex newCellID, World* newWorld, ObjectProcessorDir* processorDir, RenderQueue* newRenderQueue);
        Cell();
        //Used to allow pooling
        void init(CellIndex newCellID, World* newWorld, ObjectProcessorDir* processorDir, RenderQueue* newRenderQueue);

        //Called just before a cell is removed from its pool (manually by World)
        //Used to save any changes made to this cell for persistence
        void onDestroy();
        
        ~Cell();
        
        ObjectManager* getObjectManager();
        CellIndex getCellID();

        //Returns the value of changed, which is set to true of there are changes
        //that should be saved
        bool hasChanges();
        //Returns the value of generated, which is set to true if this Cell
        //was generated (if false, the Cell was probably loaded from file)
        bool wasGenerated();
        
        //Used by ObjectManager because accessing world causes circular reference stuff :(
        Cell* getNeighborCell(CellIndex index);
        CellIndex* getWorldIntersectingCells(Coord& topLeftCorner, float width, float height, int& size);
        
        //Loads everything a Cell needs. Returns false if
        //any cell files were not found
        //This is a LEGACY function; use loadTilesFromRLEmap instead
        bool load(const std::string& filename);
        //Loads a RLE map
        bool loadTilesFromRLEmap(const std::string& filename);
        
        //Saves everything a Cell has
        //TODO: Once editTile functions are made, keep a changed bool
        //that will be checked so that freshly generated tiles are not
        //saved
        bool save(int worldID, dynamicMultilayerMap* map);

        //Saves the map tiles using Run Length Encoding for compression.
        //All layers are saved in a single file
        bool saveTilesAsRLEmap(const std::string& filename);
        
        //Generates a new cell with the specified algorithm
        //Algorithms:
        //  2 = simplex noise expecting 255 tile values
        //  3 = simplex noise expecting 13 tile values (limited)
        void generate(int worldID, int seed, int algorithm);

        //Returns the tile at the x and y and layer; returns NULL if
        //the provided values are out of range or the tile doesn't exist
        tile* getTileAt(int tileX, int tileY,  int layer);
        //Takes a value and converts it to the tile value (value % TILE_WIDTH
        //or TILE_HEIGHT, depending on bool isX)
        unsigned int pointToTileValue(float value, bool isX);

        //Returns the difficulty of this cell, determined by the amount
        //of ground tiles that are unwalkable and the amount of onground
        //spots that are not empty. This is calculated by simply doing
        //onGroundPercentage + unwalkablePercentage and normalizing to 1.
        //Very difficult tiles are near 1 while very easy tiles are near 0
        float getDifficulty();
        
        //Renders objects relative to the provided coordinates
        //Note that render() calls renderObjects automatically
        void renderObjects(float viewX, float viewY, window* win);
        void renderBottom(tileCamera& cam, float viewX, float viewY, dynamicMultilayerMap* map, window* win);
        void renderMiddle(tileCamera& cam, float viewX, float viewY, dynamicMultilayerMap* map, window* win);
        void renderTop(tileCamera& cam, float viewX, float viewY, dynamicMultilayerMap* map, window* win);
        //Cam should be relative to the cell (use getCellOffsetX instead
        //of getTrueX) UPDATE: That comment might not be right; see world.cpp
        //ViewX and Y are only used for renderObjects()
        void render(tileCamera& cam, float viewX, float viewY, dynamicMultilayerMap* map, window* win);
        void update(Time* globalTime);

        //DELETEME
        bool testRLESaveLoadFunctions(const std::string& filename);
        
};
#endif



