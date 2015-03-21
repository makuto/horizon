#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/input/input.hpp>
//#include <base2.0/tileMap/multilayerMap.hpp>
#include "../utilities/simplexnoise.h"

int main()
{
    window win(1024, 600, "Terrain Testing");
    sprite output;
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
                    //scale  = scaled_octave_noise_3d(8, 0.55, 0.001, 0, 5, x, y, seed);
                    float value = 0;
                    if (n % 40<=30 || n < (width / 2)) value = scaled_octave_noise_3d(8, 0.55, scale, 0, 254, x, y, seed);
                    else value = scaled_octave_noise_3d(4, 0.55, scale, 0, 254, x, y, seed);
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
                                    float treeValue = scaled_octave_noise_3d(1, 0.55, treeScale, 0, 50, x, y, seed + 9594);
                                    ///use another layer of noise to get less connected trees
                                    treeValue += scaled_octave_noise_3d(1, 0.55, treeScale, 0, 30, x, y, seed + 4883);

                                    biomeValue = (3 * 32) + ((int)value % 32);
                                    
                                    if (treeValue > LEAF_VALUE) biomeValue = 128;
                                    if (treeValue > TRUNK_VALUE) biomeValue = 35;
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
