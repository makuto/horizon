#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/input/input.hpp>
#include <base2.0/noise/noise.hpp>

int SEED = 3245436;
//Override old noise function
float scaled_octave_noise_3d(float octaves, float persistence, float scale,
float loBound, float hiBound, float x,float y, float z)
{
    static Noise2d noise(SEED);
    return noise.scaledOctaveNoise2d(x, y, loBound, hiBound, octaves, scale, persistence, 2);
}
int main()
{
    window win(1024, 600, "Terrain Testing");
    sprite output;
    Noise2d noise(SEED);
    int width = 1024;
    int height = 600;
    sf::Uint8* pixels = new sf::Uint8[width * height * 4];
    float offsetX = 0;
    float offsetY = 0;
    
    for (int i = 0; i < height; ++i)
    {
        for (int n = 0; n < width; n++)
        {
            float xdiv = 200;
            float ydiv = 200;
            //float div = changingValue;
            float x = (n / xdiv) + offsetX;
            float y = (i / ydiv) + offsetY;
            float scale = 5;
            float seed = 19923;
            //scale  = scaled_octave_noise_3d(8, 0.55, 0.001, 0, 5, x, y, seed);
            float value = scaled_octave_noise_3d(8, 0.55, scale, 0, 255, x, y, seed);
            
            pixels[(i * width * 4) + (n * 4)] = value;
            pixels[(i * width * 4) + (n * 4) + 1] = value;
            pixels[(i * width * 4) + (n * 4) + 2] = value;
            pixels[(i * width * 4) + (n * 4) + 3] = 255;
            //counter+=4;
        }
    }
    //out
    sf::Texture tex;
    tex.create(width, height);
    tex.update(pixels);
    sf::Sprite* spr = output.getBase();
    spr->setTexture(tex);
    sf::Image lookup;
    if (!lookup.loadFromFile("../utilities/terrainTest.png")) return -1;
    inputManager in(&win);
    float scale = 2;
    float scaleSpeed = 0.01;
    float speed = 20 * scale;
    bool changed = true;
    float seed = 19923;
    while (!win.shouldClose())
    {
        if (in.isPressed(inputCode::Up))
        {
            changed = true;
            scale += scaleSpeed;
            //scaleSpeed = scale * scaleSpeed;
        }
        if (in.isPressed(inputCode::Down))
        {
            changed = true;
            scale -= scaleSpeed;
            //scaleSpeed = scale * scaleSpeed;
        }
        if (in.isPressed(inputCode::Space))
        {
            changed = true;
            seed+= 0.01;
        }
        if (in.isPressed(inputCode::W))
        {
            changed = true;
            offsetY -= speed;
        }
        if (in.isPressed(inputCode::S))
        {
            changed = true;
            offsetY += speed;
        }
        if (in.isPressed(inputCode::A))
        {
            changed = true;
            offsetX -= speed;
        }
        if (in.isPressed(inputCode::D))
        {
            changed = true;
            offsetX += speed;
        }
        if (changed)
        {
            for (int i = 0; i < height; ++i)
            {
                for (int n = 0; n < width; n++)
                {
                    float xdiv = 400;
                    float ydiv = 400;
                    //float div = changingValue;
                    float x = (n / xdiv) + offsetX;
                    float y = (i / ydiv) + offsetY;
                    float newScale  = scaled_octave_noise_3d(8, 0.55, 0.01, 0, 5, x, y, seed);
                    //float borderSubtract = (1 - (newScale - truncf(newScale))) * 20;
                    //Interpolate current scale and new scale values

                    float value;
                    float transitionVal = newScale - truncf(newScale);
                    float LOW_TRANSITION_START = 0.1;
                    float HIGH_TRANSITION_START = 1 - LOW_TRANSITION_START;
                    if (transitionVal < LOW_TRANSITION_START) //Transition down a scale
                    {
                        //Get values for both scales
                        float scale1 = truncf(newScale);
                        float largeValue;
                        if (n % 40<=30 || n < (width / 2)) largeValue = scaled_octave_noise_3d(8, 0.55, scale1, 0, 254, x, y, seed);
                        else largeValue = scaled_octave_noise_3d(4, 0.55, scale1, 0, 254, x, y, seed);
                        float scale2 = truncf(newScale) - 1;
                        if (scale2 < 0) scale2 = 0;
                        float smallValue;
                        if (n % 40<=30 || n < (width / 2)) smallValue = scaled_octave_noise_3d(8, 0.55, scale2, 0, 254, x, y, seed);
                        else smallValue = scaled_octave_noise_3d(4, 0.55, scale2, 0, 254, x, y, seed);
                        //Interpolate values
                        //Linear interpolation: x=(1-t)a + tb where A & B are points
                        float interp = transitionVal * (1 / LOW_TRANSITION_START); //Make transition 0 - 1 (for eq)
                        //Only go halfway (the up scale will get the rest)
                        interp /= 2;
                        interp += 0.5;
                        //largeValue = 144;
                        //smallValue = 0;
                        value = ((1-interp) * smallValue) + (interp * largeValue);
                        //value = 250;
                        
                    }
                    else if (transitionVal > HIGH_TRANSITION_START) //Transition up a scale
                    {
                        //Get values for both scales
                        float scale1 = truncf(newScale) + 1;
                        float largeValue;
                        if (n % 40<=30 || n < (width / 2)) largeValue = scaled_octave_noise_3d(8, 0.55, scale1, 0, 254, x, y, seed);
                        else largeValue = scaled_octave_noise_3d(4, 0.55, scale1, 0, 254, x, y, seed);
                        float scale2 = truncf(newScale);
                        if (scale2 < 0) scale2 = 0;
                        float smallValue;
                        if (n % 40<=30 || n < (width / 2)) smallValue = scaled_octave_noise_3d(8, 0.55, scale2, 0, 254, x, y, seed);
                        else smallValue = scaled_octave_noise_3d(4, 0.55, scale2, 0, 254, x, y, seed);
                        //Interpolate values
                        //Linear interpolation: x=(1-t)a + tb where A & B are points
                        float interp = (1 - transitionVal) * (1 / LOW_TRANSITION_START); //Make transition 0 - 1 (for eq)
                        //Only go halfway (the up scale will get the rest)
                        interp /= 2;
                        interp += 0.5;
                        //largeValue = 0;
                        //smallValue = 144;
                        value = ((1-interp) * largeValue) + (interp * smallValue);
                        //value = 0;
                    }
                    else //No transition (normal)
                    {
                        float scale1 = truncf(newScale);
                        if (n % 40<=30 || n < (width / 2)) value = scaled_octave_noise_3d(8, 0.55, scale1, 0, 254, x, y, seed);
                        else value = scaled_octave_noise_3d(4, 0.55, scale1, 0, 254, x, y, seed);
                    }
        
                    float lookupY = 0;
                    //Biomes
                    {
                        const float LAND_START = 144;
                        const float LAND_END = 176;
                        const float DRY_PRECIPITATION = 30;
                        const float FREEZING_TEMP = 30;
                        const float DESERT_TEMP = 70;
                        const float FOREST_PRECIPITATION = 60;
                        const float JUNGLE_PRECIPITATION = 80;
                        float precipitationScale = 5 * scale;
                        float temperatureScale = 1 * scale;
                        float treeScale = 100 * scale;
                        if (value >= LAND_START && value <= LAND_END)
                        {
                            /*float seed = 83434; 
                            float biomeValue = scaled_octave_noise_3d(8, 0.55, scale, 0, 5, x, y, seed);
                            biomeValue = truncf(biomeValue) * 32;
                            value = biomeValue + ((value - 143) / 32);
                            if (value > 176) value = 176;
                            lookupY = 1;*/
                            //There are 5 bands of 32 values
                            float precipitation = scaled_octave_noise_3d(8, 0.55, precipitationScale, 0, 100, x, y, seed + 1243);
                            float temperature = scaled_octave_noise_3d(8, 0.55, temperatureScale, 0, 100, x, y, seed + 8996);
                            float biomeValue = 0;
                            bool forest = false;
                            if (precipitation < DRY_PRECIPITATION || temperature > DESERT_TEMP)
                            {
                                biomeValue = 1; //Desert
                            }
                            if (temperature < FREEZING_TEMP)
                            {
                                biomeValue = 5; //Tundra
                            }
                            if (temperature > FREEZING_TEMP && precipitation > DRY_PRECIPITATION)
                            {
                                if (precipitation > FOREST_PRECIPITATION)
                                {
                                    forest = true;
                                    const float TRUNK_VALUE = 70;
                                    const float LEAF_VALUE = 50;
                                    biomeValue = 3 * 32; //Forest
                                    //Generate trees
                                    /*float treeValue = scaled_octave_noise_3d(1, 0.55, treeScale, 0, 50, x, y, seed + 9594);
                                    ///use another layer of noise to get less connected trees
                                    treeValue += scaled_octave_noise_3d(1, 0.55, treeScale, 0, 30, x, y, seed + 4883);

                                    biomeValue = (3 * 32) + ((int)value % 32);
                                    
                                    if (treeValue > LEAF_VALUE) biomeValue = 128;
                                    if (treeValue > TRUNK_VALUE) biomeValue = 35;*/
                                }
                                if (precipitation > JUNGLE_PRECIPITATION)
                                {
                                    biomeValue = 2; //Jungle
                                }
                            }
                            if (!forest) value = (biomeValue * 32) + ((int)value % 32);
                            else value = biomeValue; //Allow forest exact value
                            lookupY = 1;
                        }
                    }

                    //GRID
                    const int GRID_SIZE = 16;
                    if (i % GRID_SIZE == 0 && n % GRID_SIZE == 0)
                        value = 255;
                        
                    sf::Color colVal = lookup.getPixel(value, lookupY);
                    pixels[(i * width * 4) + (n * 4)] = colVal.r;
                    pixels[(i * width * 4) + (n * 4) + 1] = colVal.g;
                    pixels[(i * width * 4) + (n * 4) + 2] = colVal.b;
                    pixels[(i * width * 4) + (n * 4) + 3] = 255;
                    //counter+=4;

                }
            }
            tex.update(pixels);
            changed = false;
        }
        //win.getBase()->draw(*spr);
        win.draw(&output);
        win.update();
    }
    return 1;
}
