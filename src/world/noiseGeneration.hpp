#ifndef NOISEGENERATION_HPP
#define NOISEGENERATION_HPP
#include <base2.0/ept/eptParser.hpp>
#include "../utilities/simplexnoise.h"
#include "coord.hpp"

//Settings to tell the terrain generation algorithm how to convert from
//noise values to Tiles
struct TileConversionSettings
{
    
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

    //Settings for how to convert from simplex value to tile 
    TileConversionSettings tileConversionSettings;
    
    eptFile* spec;          //The spec where this information came from
};

//Creates a TerrainGenSettings from a .ept spec.
TerrainGenSettings createTerrainGenSettingsFromSpec(eptFile* spec, float seed);

//Wrapper funcs for simplex noise (TODO: MOVE TO BASE, DAMMIT!)
float scaledOctaveNoise3d(float octaves, float persistence, float scale, float x, float y, float z);

//Generate a single Cell's tiles
void generateCellTerrain(CellIndex cellIndex, std::vector<std::vector<tile> >* tiles, TerrainGenSettings* terrainGenSettings);
#endif
