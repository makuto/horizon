#include <iostream>
#include <cmath>
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/input/input.hpp>
#include <base2.0/tileMap/multilayerMap.hpp>
#include "simplexnoise.h"

int main()
{
    window win(1024, 600, "Terrain");
    win.setBackgroundColor(100, 100, 100, 255);
    sprite tileSheet;
    if (!tileSheet.load("terrainTest.png")) return -1;
    multilayerMap map;
    if (!map.load("terrainTest.map", 1)) return -1;
    map.setImage(&tileSheet);
    map.getMasterMap()->setViewSize(256, 256);
    //map.setViewSize(1024, 600);
    //std::vector<tile*>* layer = map.getLayer(0);
    std::vector<tile*>* layer = map.getMasterMap()->getMap();
    if (layer==NULL) return -1;

    for (int i = 1; i < 256; ++i)
    {
        for (int n = 1; n < 256; ++n)
        {
            tile* currentTile = (*layer)[(i * 1024) + n];
            //currentTile->x = n % 255;
            float div = 200;
            float x = n / div;
            float y = i / div;
            float value = scaled_octave_noise_2d(5, 0.5, 1, 0, 255, x, y);
            if (value < 150) value = 0;
            currentTile->x = value;
        }
    }
    inputManager in(&win);
    float changingValue = 0;
    float changingValue2 = 200;
    float changingValue3 = 2;
    float changingValue4 = 10;
    float offsetX = 0;
    float offsetY = 0;
    float rate = 10;
    float speed = 0.8;
    while (!win.shouldClose())
    {
        if (in.isPressed(inputCode::Escape)) break;
        if (in.isPressed(inputCode::Up) || in.isPressed(inputCode::Down) || in.isPressed(inputCode::Left) || in.isPressed(inputCode::Right) || in.isPressed(inputCode::RControl) || in.isPressed(inputCode::LControl) || in.isPressed(inputCode::W) || in.isPressed(inputCode::S) || in.isPressed(inputCode::A) || in.isPressed(inputCode::D) || in.isPressed(inputCode::LShift))
        {
            if (in.isPressed(inputCode::Up)) changingValue4 += 1;
            else if (in.isPressed(inputCode::Down)) changingValue4 -= 1;
            if (in.isPressed(inputCode::Left)) changingValue2 += rate;
            else if (in.isPressed(inputCode::Right)) changingValue2 -= rate;
            if (in.isPressed(inputCode::RControl)) changingValue3 +=0.1;
            if (in.isPressed(inputCode::LControl)) changingValue3 -=0.1;
            if (in.isPressed(inputCode::W)) offsetY -= speed;
            if (in.isPressed(inputCode::S)) offsetY += speed;
            if (in.isPressed(inputCode::A)) offsetX -= speed;
            if (in.isPressed(inputCode::D)) offsetX += speed;
            rate = changingValue4;
            //if (in.isPressed(inputCode::RShift)) std::cout << "new val: " << changingValue;
            std::cout << "minX: " << (1 / changingValue2) << " maxX: " << (255 / changingValue2) << " scale: "
            << changingValue3 << "\n"; 
            for (int i = 1; i < 256; ++i)
            {
                for (int n = 1; n < 256; ++n)
                {
                    tile* currentTile = (*layer)[(i * 1024) + n];
                    float tileY = 0;
                    //currentTile->x = n % 255;
                    float xdiv = changingValue2;
                    float ydiv = changingValue2;
                    //float div = changingValue;
                    float x = (n / xdiv) + offsetX;
                    float y = (i / ydiv) + offsetY;
                    float scale = 5;
                    float seed = 19923;
                    //scale  = scaled_octave_noise_3d(8, 0.55, 0.001, 0, 5, x, y, seed);
                    float value = scaled_octave_noise_3d(8, 0.55, scale, 0, 255, x, y, seed);
                    /*float scaleVal = scaled_octave_noise_3d(8, 0.55, 0.25, 1, 255, x, y, seed - 10000);
                    if (scaleVal < 150) scaleVal = 0;
                    //else scaleVal /= 3;
                    //value += scaleVal;
                    if (scaleVal != 0)
                    {
                        scaleVal-=150;
                        scaleVal *= 140;
                        scaleVal /= 155;
                        scaleVal += 147;
                        scaleVal = scaled_octave_noise_3d(8, 0.55, scale, 0, 90, x, y, seed - 10000);
                        scaleVal+=136;
                        value = scaleVal;
                    }
                    //if (value > 177) value = 167;
                    if (in.isPressed(inputCode::RShift)) value = scaled_octave_noise_3d(8, 0.55, 1, 1, 155, x, y, seed - 10000);*/
                    if (value < 0) value = 0;
                    if (value > 254) value = 254;
                    if (value > 142) //Winter
                    {
                        //value += 100 / (((int)(y+1) % 100) + 1);
                        //int yInt = (int) y;
                        float factor = fabs(sin(y * scale));
                        factor -= changingValue3 - factor; //1.3
                        //std::cout << changingValue3;
                        if (factor < 0) factor = 0;
                        value += 200 * factor; //Winter climates
                    }
                    if (value > 254) value = 254;
                    if (value < changingValue) value = 0;
                    if (value > 144 && value < 177 && in.isPressed(inputCode::LShift)) //Biomes
                    {
                        float seed = 83434; 
                        float biomeValue = scaled_octave_noise_3d(8, 0.55, scale, 0, 5, x, y, seed);
                        biomeValue = truncf(biomeValue) * 32;
                        value = biomeValue + ((value - 143) / 32);
                        if (value > 176) value = 176;
                        tileY = 1;
                    }
                    currentTile->x = value;
                    currentTile->y = tileY;
                }
            }
            map.render(0, 0, 0, 0, &win);
            win.update();
        }
    }
    return 1;
}
