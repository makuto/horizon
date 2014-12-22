#include <iostream>
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

#include "utilities/simplexnoise.h"

int main()
{
    ObjectProcessorDir testDir;
    ObjectProcessor* test2 = new ObjectProcessor();
    //test2->initialize(
    testDir.addObjProcessor(test2, 1);
    
    CellIndex testID;
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
    
    window win(1024, 600, "Horizon");
    win.setBackgroundColor(100, 100, 100, 100);
    multilayerMap defaultMap;
    if (!defaultMap.load(parser.getAttribute("files.worldDefaults.defaultMap"), 3)) return -1;
    sprite tileSet;
    if (!tileSet.load(parser.getAttribute("files.worldDefaults.tileSet").c_str())) return -1;
    defaultMap.setImage(&tileSet);
    int worldToLoad = 0;
    World newWorld(&win, &defaultMap, worldToLoad, &testDir);
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
    ObjectManager test(&newWorld, &testDir, testID, NULL);
    Object* testObj = test.getNewInitializedObject(1, 1, 1, 1, 1);
    testObj->type=1;
    Coord newPos;
    //newPos.setPosition(index, 0, 10);
    //testObj->setPosition(newPos, test);
    
    eptGroup* filesToLoad = parser.getGroup("files.testScripts");
    //Parse all files in files.testScripts
    std::string attrName;
    std::string currentFileName = filesToLoad->getAttributeFromIndex(0, attrName);
    for (int i = 1; currentFileName!=""; ++i)
    {
        if (!parser.load(currentFileName.c_str())) return -1;
        currentFileName = filesToLoad->getAttributeFromIndex(i, attrName);
    }
    
    //TODO: Convert to class?
    NeedProcessorDir needProcessorDir;
    needProcessorDir[1] = new NeedProcessor(parser.getFile("1_spec"));
    Species testSpecies(parser.getFile("testSpecies"), &needProcessorDir);

    ProcessMap processMap;
    processMap.addProcess("useItem", new Process);
    ProcessDirectory processDir(&parser, parser.getFile("needDirectory"), &processMap);
    Agent* testAgent = testSpecies.createAgent();

    inputManager in(&win);
    timer frameTime;
    frameTime.start();
    timer worldTime;
    worldTime.start();
    Time globalTime;
    Time previousUpdate;
    Time deltaTime;
    
    sprite testSprite;
    if (!testSprite.load("data/agent.png")) return -1;
    CellIndex agentCell;
    agentCell.x = 0;
    agentCell.y = 0;
    //testAgent->worldPosition.setPosition(agentCell, 100, 100);
    Coord windowPosition;
    windowPosition.setPosition(agentCell, 0, 0);
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
    for (int i = 0; i < 100; ++i)
    {
        srand(i + 1);
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
    
    //win.shouldClear(false);
    //Main loop
    while (!win.shouldClose() && !in.isPressed(inputCode::Return) && !in.isPressed(inputCode::Escape))
    {
        prof.startTiming("frame");
        //Move view
        //TODO: Why is it so choppy? :(
        float avgFrameTime = avgFrameTiming->average; //Average frame time (profiler)
        //float avgFrameTime = 0.016; //http://gafferongames.com/game-physics/fix-your-timestep/ ?
        //float avgFrameTime = frameTime.getTime(); //Current frame time
        if (in.isPressed(inputCode::LShift))
        {
            viewSpeed = defaultViewSpeed * 10;
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
        
        testSprite.setPosition(testAgent->worldPosition.getScreenX(&windowPosition), testAgent->worldPosition.getScreenY(&windowPosition));
        prof.startTiming("render");
        newWorld.render(windowPosition);
        ///////////////////////
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
        win.update();
        prof.stopTiming("render");
        
        //globalTime.addMilliseconds(frameTime.getTime());
        globalTime.reset();
        globalTime.addSeconds(worldTime.getTime());
        //std::cout << worldTime.getTime() << "\n";
        previousUpdate.getDeltaTime(&globalTime, deltaTime);
        //previousUpdate = globalTime;
        if (deltaTime.getExactSeconds()>=0.016)
        {
            prof.startTiming("updateAgent");
            testSpecies.updateAgent(testAgent, &globalTime, &deltaTime, &processDir);
            prof.stopTiming("updateAgent");
            prof.startTiming("updateWorld");
            newWorld.update(windowPosition, &globalTime, MAX_WORLD_FAR_UPDATE);
            prof.stopTiming("updateWorld");
            previousUpdate = globalTime;
            //globalTime.print();
        }
        prof.stopTiming("frame");
    }
    //Remember to do this!
    delete testAgent;
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
