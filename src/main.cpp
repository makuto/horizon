#include <iostream>
#include <cmath>
#include <base2.0/ept/eptParser.hpp>
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/input/input.hpp>
#include <base2.0/timer/timer.hpp>
#include <base2.0/profiler/profiler.hpp>
#include "agent/species.hpp"
#include "agent/needProcessor.hpp"
#include "agent/agent.hpp"
#include "agent/process.hpp"
#include "agent/processDirectory.hpp"
#include "agent/processMap.hpp"

#include "world/world.hpp"
#include "world/cell.hpp"

#include "object/objectManager.hpp"
#include "object/objectProcessorDir.hpp"
#include "object/objectProcessor.hpp"
#include "object/processors/agentProcessor.hpp"

#include "utilities/simplexnoise.h"

#include "utilities/debugText.hpp"

#include "world/path.hpp"

void test()
{
    window win(1024, 600, "eest");
    dynamicMultilayerMap testMap(10, 10, 2);
    dynamicTileMap* masterMap = testMap.getMasterMap();
    masterMap->setTileSize(32, 32);
    masterMap->setViewSize(10, 10);
    //masterMap->setMapSize(10, 10);
    std::vector<tile> layer1;
    layer1.resize(100);
    for (int i = 0; i < 100; ++i)
    {
        layer1[i].x = 254;
        layer1[i].y = 0;
    }
    std::vector<tile> layer2;
    layer2.resize(100);
    for (int i = 0; i < 100; ++i)
    {
        if (i % 3 == 0)
        {
            layer2[i].x = 100;
            layer2[i].y = 0;
        }
        else
        {
            layer2[i].x = 255;
            layer2[i].y = 255;
        }
    }
    testMap.setLayer(0, &layer1);
    testMap.setLayer(1, &layer2);
    sprite tileSet;
    if (!tileSet.load("data/images/terrain.png")) return;
    testMap.setImage(&tileSet);
    while(!win.shouldClose())
    {
        testMap.render(0, 1, 0, 0, &win);
        win.update();
    }
}
int main()
{
    //test();
    
    //CellIndex testID;
    /*
    testID.x = 1;
    testID.y = 1;
    ObjectManager testManager(NULL, &testDir, testID, NULL);
    Object* testObj2 = testManager.getNewRawObject(1, 1230, 2000);
    testManager.getNewRawObject(1, 1230, 2000);
    testManager.getNewRawObject(1, 1230, 200);
    testManager.getNewInitializedObject(1, 1, 1230, 21, 0);
    testObj2->type = 1;
    aabb testRange(0, 0, 2048, 2000);
    std::vector<Object*>* results = testManager.getObjectsInRange(testRange);
    if (!results) return -1;
    for (std::vector<Object*>::iterator it = results->begin(); it != results->end(); ++it)
    {
        std::cout << (*it)->type << " pos: ";
        (*it)->getPosition().print();
        std::cout << "\n";
    }
    delete results;*/
    //return 1;
    eptParser parser;
    if(!parser.load("data/files.ept")) return -1;
    eptGroup* filesToLoad = parser.getGroup("files.scripts");
    //Parse all files in files.testScripts
    std::string attrName;
    std::string currentFileName = filesToLoad->getAttributeFromIndex(0, attrName);
    for (int i = 1; currentFileName!=""; ++i)
    {
        if (!parser.load(currentFileName.c_str()))
        {
            std::cout << "Failed to load script " << currentFileName << "!/n";
            return -1;
        }
        std::cout << "Loaded script " << currentFileName << " successfully\n";
        currentFileName = filesToLoad->getAttributeFromIndex(i, attrName);
    }
    //if(!parser.load("data/scripts/testObject.ept")) return -1;
    window win(1024, 600, "Horizon");
    win.setBackgroundColor(100, 100, 100, 100);
    inputManager in(&win);

    //TODO: Convert to class?
    NeedProcessorDir needProcessorDir;
    needProcessorDir[1] = new NeedProcessor(parser.getFile("1_spec"));
    Species testSpecies(parser.getFile("testSpecies"), &needProcessorDir, 1);

    ProcessMap processMap;
    processMap.addProcess("useItem", new Process);
    ProcessDirectory processDir(&parser, parser.getFile("needDirectory"), &processMap);
    
    //multilayerMap defaultMap;
    //if (!defaultMap.load(parser.getAttribute("files.worldDefaults.defaultMap"), 3)) return -1;
    sprite tileSet;
    if (!tileSet.load(parser.getAttribute("files.worldDefaults.tileSet").c_str())) return -1;
    int worldToLoad = 0;
    //World newWorld(&win, &defaultMap, worldToLoad, &testDir);
    dynamicMultilayerMap dynamicMap(CELL_HEIGHT, CELL_WIDTH, NUM_LAYERS);
    dynamicTileMap* dynamicMasterMap = dynamicMap.getMasterMap();
    dynamicMasterMap->setTileSize(TILE_WIDTH, TILE_HEIGHT);
    dynamicMasterMap->setViewSize(win.getHeight() / TILE_HEIGHT, win.getWidth() / TILE_WIDTH);
    dynamicMasterMap->setImage(&tileSet);
    ObjectProcessorDir testDir;
    World newWorld(&win, &dynamicMap, worldToLoad, &testDir);

    PathManager pathManager(&newWorld);
    
    ObjectProcessor* test2 = new ObjectProcessor();
    test2->setup(&in, &pathManager);
    test2->initialize(parser.getFile("pickupObj"));
    AgentProcessor* agentObj = new AgentProcessor(&testSpecies, &processDir);
    agentObj->initialize(parser.getFile("agentObj"));
    testDir.addObjProcessor(test2);
    testDir.addObjProcessor(agentObj);

    Path testPath;
    Coord start;
    start.setPosition(0, 0, 1024, 2047);
    Coord end;
    end.setPosition(23, -10, 1666, 1666);
    //end.setPosition(0, 1, 512, 512);
    testPath.init(&newWorld, start, end);
    testPath.calculateCellPath();
    std::cout << testPath.getStatus() << "\n";
    testPath.calculateTilePath();
    std::cout << testPath.getStatus() << "\n";
    //Coord followResult = testPath.advance(start);
    //followResult.print();
    Coord nextPos = start;
    /*while(testPath.getStatus() != -1 && testPath.getStatus() != 2)
    {
        int tileX = nextPos.getCellOffsetX() / TILE_WIDTH;
        int tileY = nextPos.getCellOffsetY() / TILE_HEIGHT;
        Cell* currentCell = newWorld.getCell(nextPos.getCell());
        if (currentCell)
        {
            tile* currentTile = currentCell->getTileAt(tileX, tileY, 2);
            currentTile->x = 153;
            currentTile->y = 0;
        }
        nextPos = testPath.advance(nextPos);
        if (testPath.getStatus() == 0) testPath.calculateTilePath();
    }*/
    //testPath.calculateTilePath();
    //testPath.calculate(); //Macro
    //testPath.calculate(); //Micro
    //return 1;
    /*CellIndex index;
    index.x = 0;
    index.y = 0;
    if (!newWorld.loadCell(index)) return -1;
    index.x = 1;
    index.y = 1;
    if (!newWorld.loadCell(index)) return -1;
    index.x = -1;
    index.y = -1;
    if (!newWorld.loadCell(index)) return -1;
    index.x = -1;
    index.y = 0;
    if (!newWorld.loadCell(index)) return -1;*/
    //ObjectManager test(&newWorld, &testDir, testID, NULL);
    //Object* testObj = test.getNewInitializedObject(1, 1, 1, 1, 1);
    //testObj->type=1;
    //Coord newPos;
    //newPos.setPosition(index, 0, 10);
    //testObj->setPosition(newPos, test);
    
    
    //Agent* testAgent = testSpecies.createAgent(0);

    timer frameTime;
    frameTime.start();
    timer worldTime;
    worldTime.start();
    Time globalTime;
    Time previousUpdate;
    Time deltaTime;
    
    sprite testSprite;
    if (!testSprite.load("data/images/agent.png")) return -1;
    CellIndex agentCell;
    agentCell.x = 0;
    agentCell.y = 0;
    //testAgent->worldPosition.setPosition(agentCell, 100, 100);
    Coord windowPosition;
    windowPosition.setPosition(agentCell, 0, 0);
    //windowPosition.setPosition(23, -10, 0, 0);
    float defaultViewSpeed = 400;
    float viewSpeed = defaultViewSpeed;
    profiler prof;
    prof.startTiming("frame");
    timer totalTime;
    totalTime.start();
    timing* avgFrameTiming = prof.getTiming("frame");
    
    CellIndex cellToGet;
    cellToGet.x = 0;
    cellToGet.y = 0;
    Cell* originCell = newWorld.getCell(cellToGet);
    if (originCell==NULL) return -1;
    ObjectManager* originObjMan = originCell->getObjectManager();
    srand(432432);
    for (int i = 0; i < 4; ++i)
    {
        //originObjMan->getNewInitializedObject(1, 1, i * 204.8, i * 204.8, i * 36);
        originObjMan->getNewInitializedObject(1, 1, rand() % 2048, rand() % 2048, 0);
    }
    originObjMan->getNewInitializedObject(1, 2, 128, 128, 0); //Keyboard test object
    originObjMan->getNewInitializedObject(1, 3, 512, 512, 0); //Path test object
    //Agent object
    Agent* pooledAgent = testSpecies.createAgent(0);
    if (!pooledAgent) return -1;
    Object* testAgentObj = originObjMan->getNewInitializedObject(2, pooledAgent->id, 64, 64, 0);
    if (!testAgentObj) return -1;
    cellToGet.x = -1;
    cellToGet.y = -1;
    originCell = newWorld.getCell(cellToGet);
    if (originCell==NULL) return -1;
    originObjMan = originCell->getObjectManager();
    srand(4387324);
    for (int i = 0; i < 4; ++i)
    {
        //originObjMan->getNewInitializedObject(1, 1, i * 204.8, i * 204.8, i * 36);
        originObjMan->getNewInitializedObject(1, 1, rand() % 2048, rand() % 2048, 0);
    }

    ///////////////////
    sprite tileSheet;
    if (!tileSheet.load("src/utilities/terrainTest.png")) return -1;
    multilayerMap map;
    if (!map.load("src/utilities/terrainTest.map", 1)) return -1;
    map.setImage(&tileSheet);
    map.getMasterMap()->setViewSize(256, 256);
    std::vector<tile*>* layer = map.getMasterMap()->getMap();
    if (layer==NULL) return -1;
    ///////////////////

    //Used for debug text
    text textToRender;
    textToRender.setSize(14);
    textToRender.setColor(255, 38, 38, 255);
    if (!textToRender.loadFont("data/fonts/font1.ttf"))
    {
        std::cout << "err: cannot load debugText font\n";
        return -1;
    }
    textToRender.setPosition(10, 10);
    int smoothAmount = 20;
    float avgTimes[smoothAmount];
    int index = 0;
    for (int i = 0; i < smoothAmount; ++i)
    {
        avgTimes[i] = 0.016;
    }

    sprite dayNightSpr;
    std::vector<tile> dayNightData;
    dayNightData.resize(64 * 64);
    if (dayNightSpr.load("data/images/day-night.png"))
    {
        for (int i = 0; i < 64 * 64; ++i)
        {
            //dayNightData[i].x = (unsigned char)truncf(((globalTime.getSeconds() * 10000) / SECONDS_IN_DAY) * 32);
            dayNightData[i].x = 0;
            dayNightData[i].y = 0;
        }
        
    }
    //dayNightSpr.getBase()->setScale(32, 32);
    //dayNightSpr.getBase()->scale(32, 32);
        
    win.shouldClear(false);
    //win.getBase()->setVerticalSyncEnabled(false);
    //win.getBase()->setFramerateLimit(60);
    bool isTapped = false;
    bool isPressed = false;
    
    //Main loop
    while (!win.shouldClose() && !in.isPressed(inputCode::Return) && !in.isPressed(inputCode::Escape))
    {
        prof.startTiming("frame");
        //Move view
        //TODO: Why is it so choppy? :(
        float avgFrameTime = avgFrameTiming->average; //Average frame time (profiler)
        avgTimes[index] = avgFrameTime;
        index++;
        if (index >= smoothAmount) index = 0;
        float thisAvg = 0;
        for (int i = 0; i < smoothAmount; ++i)
        {
            thisAvg+=avgTimes[i];
        }
        thisAvg /= smoothAmount;
        if (thisAvg < 0) thisAvg = 0;
        DebugText::addEntry("Avg Frame Time (seconds): ", avgFrameTime);
        DebugText::addEntry("Avg Frame Time (FPS): ", 1 / avgFrameTime);
        avgFrameTime = thisAvg;
        DebugText::addEntry("Avg Frame Time 10 (seconds): ", thisAvg);
        DebugText::addEntry("Avg Frame Time (FPS): ", 1 / avgFrameTime);
        //float avgFrameTime = 0.016; //http://gafferongames.com/game-physics/fix-your-timestep/ ?
        //float avgFrameTime = frameTime.getTime(); //Current frame time
        if (in.isPressed(inputCode::LShift))
        {
            viewSpeed = defaultViewSpeed * 40;
        }
        else viewSpeed = defaultViewSpeed;
        if (in.isPressed(inputCode::Up))
        {
            windowPosition.addVector(0, -viewSpeed * avgFrameTime);
        }
        if (in.isPressed(inputCode::Down))
        {
            windowPosition.addVector(0, viewSpeed * avgFrameTime);
        }
        if (in.isPressed(inputCode::Left))
        {
            windowPosition.addVector(-viewSpeed * avgFrameTime, 0);
        }
        if (in.isPressed(inputCode::Right))
        {
            windowPosition.addVector(viewSpeed * avgFrameTime, 0);
        }
        
        //testSprite.setPosition(testAgent->worldPosition.getScreenX(&windowPosition), testAgent->worldPosition.getScreenY(&windowPosition));
        prof.startTiming("render");
        newWorld.render(windowPosition, &globalTime);
        ///////////////////////MINIMAP
        if (in.isPressed(inputCode::Space))
        {
            for (int i = 0; i < 256; ++i)
            {
                for (int n = 0; n < 256; ++n)
                {
                    tile* currentTile = (*layer)[(i * 1024) + n];
                    //currentTile->x = n % 255;
                    float xdiv = 2;
                    float ydiv = 2;
                    float noiseX = n + ((windowPosition.getTrueX() - 4096) / 32);
                    float noiseY = i + ((windowPosition.getTrueY() - 4096) / 32);
                    noiseX /= xdiv;
                    noiseY /= ydiv;
                    float x = noiseX;
                    float y = noiseY;
                    float scale = 0.001;
                    float value = scaled_octave_noise_3d(10, 0.55, scale, 0, 255, x, y, 0);
                    //Winter bands
                    if (value > 142)
                    {
                        //value += 100 / (((int)(y+1) % 100) + 1);
                        //int yInt = (int) y;
                        float factor = fabs(sin(y * scale));
                        const float SNOW_AMOUNT = 1.9;
                        factor -= SNOW_AMOUNT - factor; //1.3
                        if (factor < 0) factor = 0;
                        const float SNOW_FALLOFF = 1000;
                        value += SNOW_FALLOFF * factor; //Winter climates
                        if (value > 254) value = 254;
                        if (value < 142) value = 142;
                    }
                    
                    if (i % 64 == 0) value = 0;
                    if (n % 64 == 0) value = 0;
                    currentTile->x = value;
                }
            }
            map.render(0, 0, 0, 0, &win);
        }
        ///////////////////////
        win.draw(&testSprite);
        frameTime.start();
    
        ////Day night
        /*for (int i = 0; i < 64 * 64; ++i)
        {
            //dayNightData[i].x = (unsigned char)truncf(((globalTime.getSeconds() * 10000) / SECONDS_IN_DAY) * 32);
            dayNightData[i].x = globalTime.getSeconds() / 2;
            dayNightData[i].y = 0;
        }
        dynamicMap.setImage(&dayNightSpr);
        dynamicMap.getMasterMap()->setViewOffset(0, 0);
        dynamicMap.setLayer(0, &dayNightData);
        dynamicMap.render(0, 0, 0, 0, &win);
        dynamicMap.setImage(&tileSet);*/
        ////Day night
        
        DebugText::render(&win, &textToRender);
        DebugText::clear();
        win.update();
        prof.stopTiming("render");
        
        //globalTime.addMilliseconds(frameTime.getTime());
        //globalTime.reset();
        //globalTime.addSeconds(worldTime.getTime());
        //globalTime.addSeconds(avgFrameTime);
        globalTime.addSeconds(0.016);
        DebugText::addEntry("Global Time: ", globalTime.getExactSeconds());
        DebugText::addEntry("Window Cell Position: ", windowPosition.getCell().x, windowPosition.getCell().y);
        //std::cout << worldTime.getTime() << "\n";
        previousUpdate.getDeltaTime(&globalTime, deltaTime);
        //previousUpdate = globalTime;
        //if (deltaTime.getExactSeconds()>=0.016)
        //if (deltaTime.getExactSeconds()>=avgFrameTime)
        if (deltaTime.getExactSeconds()>=0.014)
        {
            prof.startTiming("updateAgent");
            //testSpecies.updateAgent(testAgent, &globalTime, &deltaTime, &processDir);
            prof.stopTiming("updateAgent");
            prof.startTiming("updateWorld");
            pathManager.update(0.01);
            newWorld.update(windowPosition, &globalTime, MAX_WORLD_FAR_UPDATE);
            /*if (isTapped)
            {
                newWorld.update(windowPosition, &globalTime, MAX_WORLD_FAR_UPDATE);
                isTapped = false;
            }
            else
            {
                if (!isPressed && in.isPressed(inputCode::U))
                {
                    isTapped = true;
                    isPressed = true;
                }
                else
                {
                    isTapped = false;
                }
                if (!in.isPressed(inputCode::U))
                {
                    isPressed = false;
                }
            }*/
            /*for (int i = 0; i < 2; ++i)
            {
                newWorld.update(windowPosition, &globalTime, MAX_WORLD_FAR_UPDATE);
                globalTime.addSeconds(0.008);
            }*/
            prof.stopTiming("updateWorld");
            previousUpdate = globalTime;
            //globalTime.print();
        }
        prof.stopTiming("frame");
    }
    //Remember to do this! UPDATE: Not any more - agents are pooled
    //delete testAgent;
    prof.outputAllResults();
    prof.outputAllResultsPercentages(totalTime.getTime());
    
    /*Agent testAgent;
    int difficulty;
    ProcessChain* testChain = processDir.getOptimalChain(&testAgent, NULL, 1, difficulty);
    std::cout << "Difficulty: " << difficulty << "\n";
    testAgent.currentProcessChain = new ProcessChain;
    testAgent.currentProcessChain->push_back(new Process);
    (*testAgent.currentProcessChain)[0]->update(&testAgent, NULL, NULL);*/
    return 1;
}
