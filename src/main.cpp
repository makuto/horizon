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

int main()
{
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
    World newWorld(&win, &defaultMap, worldToLoad);
    CellIndex index;
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
    if (!newWorld.loadCell(index)) return -1;
    ObjectManager test(&newWorld, NULL);
    Object* testObj = test.getNewObject(1, 1, 1);
    Coord newPos;
    newPos.setPosition(index, 0, 10);
    testObj->setPosition(newPos, test);
    
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
    testAgent->worldPosition.setPosition(agentCell, 100, 100);
    Coord windowPosition;
    windowPosition.setPosition(agentCell, 0, 0);
    float viewSpeed = 200;
    profiler prof;
    prof.startTiming("frame");
    timer totalTime;
    totalTime.start();
    timing* avgFrameTiming = prof.getTiming("frame");

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
        win.draw(&testSprite);
        frameTime.start();
        win.update();
        prof.stopTiming("render");
        
        //globalTime.addMilliseconds(frameTime.getTime());
        globalTime.reset();
        globalTime.addSeconds(worldTime.getTime());
        previousUpdate.getDeltaTime(&globalTime, deltaTime);
        //previousUpdate = globalTime;
        if (deltaTime.getExactSeconds()>=0.1)
        {
            prof.startTiming("updateAgent");
            testSpecies.updateAgent(testAgent, &globalTime, &deltaTime, &processDir);
            prof.stopTiming("updateAgent");
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
