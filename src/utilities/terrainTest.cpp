#include <iostream>
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
    float changingValue = 200;
    float changingValue2 = 200;
    float changingValue3 = 2;
    float offsetX = 0;
    float offsetY = 0;
    float rate = 10;
    while (!win.shouldClose())
    {
        if (in.isPressed(inputCode::Escape)) break;
        if (in.isPressed(inputCode::Up) || in.isPressed(inputCode::Down) || in.isPressed(inputCode::Left) || in.isPressed(inputCode::Right) || in.isPressed(inputCode::RControl) || in.isPressed(inputCode::W) || in.isPressed(inputCode::S) || in.isPressed(inputCode::A) || in.isPressed(inputCode::D))
        {
            if (in.isPressed(inputCode::Up)) changingValue += rate;
            else if (in.isPressed(inputCode::Down)) changingValue -= rate;
            if (in.isPressed(inputCode::Left)) changingValue2 += rate;
            else if (in.isPressed(inputCode::Right)) changingValue2 -= rate;
            if (in.isPressed(inputCode::RControl)) changingValue3 +=1;
            if (in.isPressed(inputCode::W)) offsetY -= 0.1;
            if (in.isPressed(inputCode::S)) offsetY += 0.1;
            if (in.isPressed(inputCode::A)) offsetX -= 0.1;
            if (in.isPressed(inputCode::D)) offsetX += 0.1;
            //if (in.isPressed(inputCode::RShift)) std::cout << "new val: " << changingValue;
            for (int i = 1; i < 256; ++i)
            {
                for (int n = 1; n < 256; ++n)
                {
                    tile* currentTile = (*layer)[(i * 1024) + n];
                    //currentTile->x = n % 255;
                    float div = changingValue2;
                    //float div = changingValue;
                    float x = (n / div) + offsetX;
                    float y = (i / div) + offsetY;
                    float value = scaled_octave_noise_2d(changingValue3, 0.55, 1, 0, 255, x, y);
                    if (value < changingValue) value = 0;
                    currentTile->x = value;
                }
            }
            map.render(0, 0, 0, 0, &win);
            win.update();
        }
    }
    return 1;
}
