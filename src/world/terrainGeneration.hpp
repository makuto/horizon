#ifndef TERRAINGENERATION_HPP
#define TERRAINGENERATION_HPP
#include <vector>
#include <base2.0/ept/eptParser.hpp>
#include <base2.0/tileMap/tileMap.hpp>
#include "coord.hpp"


struct ValueRangeTile
{
    int endValue;     //The final value associated with a tile
    tile tileToUse;
};
//Settings to tell the terrain generation algorithm how to convert from
//noise values to Tiles
struct TileConversionSettings
{
    std::vector<tile>   lookupTable;        //An array of tiles; one tile for each value
                                            //This is generated at runtime from the ranges
    std::vector<ValueRangeTile>
                        valueRangeTiles;    //The tile to use for every value startValue
                                            //(until overridden by a higher start value)
};
//Settings for the terrain generation algorithm
struct TerrainGenSettings
{
    int     algorithm;      //Which algorithm to use (TODO: Make enum)

    float   seed;           //Since seed is just put in Z, it can be float
    
    float   positionScale;  //Points are multiplied by this value before
                            //passed to the noise function
    
    float   octaves;        //Number of octaves of noise to use
    
    float   persistence;    //How much persistence between octaves of noise

    int valueRange;         //The total number of different values

    //Settings for how to convert from simplex value to tile 
    TileConversionSettings tileConversionSettings;
    
    eptFile* spec;          //The spec where this information came from
};

//Creates a TerrainGenSettings from a .ept spec.
TerrainGenSettings createTerrainGenSettingsFromSpec(eptFile* spec, float seed);

//Generate a single Cell's tiles
void generateCellTerrain(CellIndex cellIndex, std::vector<std::vector<tile> >* tiles, TerrainGenSettings* terrainGenSettings);
#endif
