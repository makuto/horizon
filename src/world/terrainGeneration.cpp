#ifndef TERRAINGENERATION_CPP
#define TERRAINGENERATION_CPP
#include "terrainGeneration.hpp"
#include <iostream>

//Creates TileConversionSettings from a .ept spec
//Returns false if something failed
bool createTileConversionSettingsFromSpec(eptFile* spec, TerrainGenSettings& terrainGenSettings, int valueRange)
{
    TileConversionSettings newConvSettings;
    newConvSettings.lookupTable.resize(valueRange);

    if (!spec)
        return false;
        
    //Load the TileConversionSettings spec
    std::string tileConversionSettingsSpecName = spec->getAttribute("_files.tileConversionSettings");
    //This is bad
    eptParser parser;
    if (!parser.load(tileConversionSettingsSpecName.c_str()))
        std::cout << "ERROR: createTileConversionSettingsFromSpec(): Couldn't load tileConversionSettings spec '"
        <<  tileConversionSettingsSpecName << "'!\n";
        return false;

    eptFile* tileConversionSettingsSpec = parser.getFile(tileConversionSettingsSpecName);

    if (!tileConversionSettingsSpec)
        return false;

    //For each group in the spec, add a new ValueRangeTile
    //Group name is the startValue (forcing proper ordering)
    std::string groupName;
    eptGroup* currentGroup = tileConversionSettingsSpec->getGroupFromIndex(0, groupName);
    for (int i = 1; currentGroup != NULL; i++)
    {
        //Prepare the ValueRangeTile
        ValueRangeTile currentValueRangeTile;
        currentValueRangeTile.tileToUse.x = attrToInt(currentGroup->getAttribute("x"));
        currentValueRangeTile.tileToUse.y = attrToInt(currentGroup->getAttribute("y"));
        currentValueRangeTile.endValue = attrToInt(groupName);

        newConvSettings.valueRangeTiles.push_back(currentValueRangeTile);
        
        currentGroup = tileConversionSettingsSpec->getGroupFromIndex(i, groupName);
    }

    //Take the value range tiles and put them in the lookup table
    int currentValue = 0;
    for (std::vector<ValueRangeTile>::iterator it = newConvSettings.valueRangeTiles.begin();
        it != newConvSettings.valueRangeTiles.end(); ++it)
    {
        tile currentTile = (*it).tileToUse;
        int endValue = (*it).endValue;
        for (; currentValue <= endValue; currentValue++)
        {
            newConvSettings.lookupTable.push_back(currentTile);
        }
    }

    if (valueRange != currentValue)
        std::cout << "WARNING: createTileConversionSettingsFromSpec(): Expected " << valueRange << " tile values; got " << currentValue << "\n";

    return true;
    
    //TODO: Throw this all away due to biomes...? (need to rethink terrain gen a bit)
}
//Creates a TerrainGenSettings from a .ept spec
TerrainGenSettings createTerrainGenSettingsFromSpec(eptFile* spec, float seed)
{
    TerrainGenSettings newTerrainGenSettings;
    int valueRange = attrToInt(spec->getAttribute("terrainGenSettings.valueRange"));
    newTerrainGenSettings.valueRange = valueRange;
    
    createTileConversionSettingsFromSpec(spec, newTerrainGenSettings, valueRange);

    newTerrainGenSettings.algorithm = attrToInt(spec->getAttribute("terrainGenSettings.algorithm"));
    newTerrainGenSettings.positionScale = attrToInt(spec->getAttribute("terrainGenSettings.positionScale"));
    newTerrainGenSettings.octaves = attrToInt(spec->getAttribute("terrainGenSettings.octaves"));
    
    
    newTerrainGenSettings.seed = seed;

    return newTerrainGenSettings;
}
float getElevationAt(CellIndex cell, int x, int y, TerrainGenSettings* terrainGenSettings)
{
    float noiseX = x + (CELL_WIDTH * cell.x);
    float noiseY = y + (CELL_HEIGHT * cell.y);
    noiseX /= 2; //TODO: Raise this value for more accurate floats far away?
    noiseY /= 2;
    const float SCALE = 0.001;
    
    //TODO: Put all these values in a text file
    //float value = scaled_octave_noise_3d(10, 0.55, SCALE, 0, 255, noiseX, noiseY, seed);
    float value = 0;
    
    return value;
}
void generateCellTerrain(int worldID, int seed, int algorithm, CellIndex cell, std::vector<std::vector<tile> >* cellTiles)
{
 /*   //I doubt there will be very many, so I'll just put it in switch
    switch (algorithm)
    {
        //Simplex noise world; expects 0-255 colors (terrain.png); no biomes
        case 2:
            for (int y = 0; y < CELL_HEIGHT; ++y)
            {
                for (int x = 0; x < CELL_WIDTH; ++x)
                {
                    float value = getElevationAt(cell, x, y);
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
                    //TEMP DELETE ME!
                    //value = 150;
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
        //Simplex noise (no biome); limited colors (3 for each range)
        case 3:
            for (int y = 0; y < CELL_HEIGHT; ++y)
            {
                for (int x = 0; x < CELL_WIDTH; ++x)
                {
                    float value = getElevationAt(cell, x, y);
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
                    //TEMP DELETE ME!
                    //value = 150;
                    if (value > 185) //Mountains are on multiple layers
                    {
                        //Ground and onground are solid black
                        tiles[0][x + (y * CELL_WIDTH)].x = 9;
                        tiles[0][x + (y * CELL_WIDTH)].y = 0;
                        tiles[1][x + (y * CELL_WIDTH)].x = 9;
                        tiles[1][x + (y * CELL_WIDTH)].y = 0;
                        //Above ground is elevation (the 0.04... maps 0 - 70 to 0 - 3)
                        float limitedValue = (value - 185) * 0.042857143;
                        limitedValue += 9;
                        tiles[2][x + (y * CELL_WIDTH)].x = (unsigned char)limitedValue;
                        tiles[2][x + (y * CELL_WIDTH)].y = 0;
                    }
                    else
                    {
                        //The 0.04.... maps 0 - 185 to 0-8
                        float limitedValue = value * 0.043243243;
                        tiles[0][x + (y * CELL_WIDTH)].x = (unsigned char)limitedValue;
                        tiles[0][x + (y * CELL_WIDTH)].y = 0;
                        //Empty air for all other biomes
                        tiles[1][x + (y * CELL_WIDTH)].x = 255;
                        tiles[1][x + (y * CELL_WIDTH)].y = 255;
                        tiles[2][x + (y * CELL_WIDTH)].x = 255;
                        tiles[2][x + (y * CELL_WIDTH)].y = 255;
                    }
                }
            }
            std::cout << "Done generating " << tiles.size() << " of " << NUM_LAYERS << " layers addr " << &tiles[0] << " \n";
            break;
        default:
            std::cout << algorithm << " is not a valid algorithm (Cell.generate())\n";
            break;
    }
    calculateDifficulty();
    generated = true;*/
}
#endif
