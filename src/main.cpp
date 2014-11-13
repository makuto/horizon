#include <iostream>
#include "agent/species.hpp"
#include "agent/needProcessor.hpp"
#include "agent/agent.hpp"
#include "agent/process.hpp"
#include "agent/processDirectory.hpp"
#include "agent/processMap.hpp"
#include <base2.0/ept/eptParser.hpp>

int main()
{
    eptParser parser;
    parser.load("data/LOCAL_test.ept");
    parser.load("data/testNeed.ept");
    parser.load("data/needDirectory.ept");
    std::map<std::string, NeedProcessor*> needProcessorDir;
    needProcessorDir["need"] = new NeedProcessor(parser.getFile("1_spec"));
    Species testSpecies(parser.getFile("testSpecies"), &needProcessorDir);

    //TODO: Make this a class? At least a typedef
    ProcessMap processMap;
    processMap.addProcess("useItem", new Process);
    ProcessDirectory processDir(&parser, parser.getFile("needDirectory"), &processMap);
    testSpecies.updateAgent(testSpecies.createAgent(), new Time, &processDir);
    Agent testAgent;
    ProcessChain* testChain = processDir.getOptimalChain(&testAgent, NULL, 1);
    if (testChain==NULL) std::cout << "No optimal chain found\n";
    testAgent.currentProcessChain = new ProcessChain;
    testAgent.currentProcessChain->push_back(new Process);
    (*testAgent.currentProcessChain)[0]->update(&testAgent, NULL, NULL);
    return 1;
}
