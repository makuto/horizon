#include <iostream>
#include <base2.0/ept/eptParser.hpp>
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/input/input.hpp>
#include <base2.0/timer/timer.hpp>
#include "agent/species.hpp"
#include "agent/needProcessor.hpp"
#include "agent/agent.hpp"
#include "agent/process.hpp"
#include "agent/processDirectory.hpp"
#include "agent/processMap.hpp"

int main()
{
    eptParser parser;
    parser.load("data/LOCAL_test.ept");
    parser.load("data/testNeed.ept");
    parser.load("data/needDirectory.ept");
    //TODO: Convert to class?
    std::map<std::string, NeedProcessor*> needProcessorDir;
    needProcessorDir["need"] = new NeedProcessor(parser.getFile("1_spec"));
    Species testSpecies(parser.getFile("testSpecies"), &needProcessorDir);

    ProcessMap processMap;
    processMap.addProcess("useItem", new Process);
    ProcessDirectory processDir(&parser, parser.getFile("needDirectory"), &processMap);
    Agent* testAgent = testSpecies.createAgent();

    window win(1024, 600, "Horizon");
    win.setBackgroundColor(100, 100, 100, 100);
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
    testAgent->worldPosition.setPosition(100, 100);
    Coord windowPosition;
    windowPosition.setPosition(0, 0);
    float viewSpeed = 200;
    while (!win.shouldClose() && !in.isPressed(inputCode::Return))
    {
        //Move view
        if (in.isPressed(inputCode::Up))
        {
            windowPosition.addVector(0, -viewSpeed * frameTime.getTime());
        }
        if (in.isPressed(inputCode::Down))
        {
            windowPosition.addVector(0, viewSpeed * frameTime.getTime());
        }
        if (in.isPressed(inputCode::Left))
        {
            windowPosition.addVector(-viewSpeed * frameTime.getTime(), 0);
        }
        if (in.isPressed(inputCode::Right))
        {
            windowPosition.addVector(viewSpeed * frameTime.getTime(), 0);
        }
        
        testSprite.setPosition(testAgent->worldPosition.getScreenX(&windowPosition), testAgent->worldPosition.getScreenY(&windowPosition));
        win.draw(&testSprite);
        frameTime.start();
        win.update();
        
        //globalTime.addMilliseconds(frameTime.getTime());
        globalTime.reset();
        globalTime.addMilliseconds(worldTime.getTime());
        previousUpdate.getDeltaTime(&globalTime, deltaTime);
        deltaTime.invert();
        if (deltaTime.seconds>=1)
        {
            testSpecies.updateAgent(testAgent, &deltaTime, &processDir);
            previousUpdate = globalTime;
            previousUpdate.print();
            globalTime.print();
        }
    }
    /*Agent testAgent;
    int difficulty;
    ProcessChain* testChain = processDir.getOptimalChain(&testAgent, NULL, 1, difficulty);
    std::cout << "Difficulty: " << difficulty << "\n";
    testAgent.currentProcessChain = new ProcessChain;
    testAgent.currentProcessChain->push_back(new Process);
    (*testAgent.currentProcessChain)[0]->update(&testAgent, NULL, NULL);*/
    return 1;
}
