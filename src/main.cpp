#include <iostream>
#include "agent/species.hpp"
#include "agent/needProcessor.hpp"
#include "agent/agent.hpp"
#include "agent/process.hpp"
#include <base2.0/ept/eptParser.hpp>

int main()
{
    eptParser parser;
    parser.load("data/LOCAL_test.ept");
    parser.load("data/testNeed.ept");
    std::map<std::string, NeedProcessor*> needProcessorDir;
    needProcessorDir["need"] = new NeedProcessor(parser.getFile("need"));
    Species testSpecies(parser.getFile("testSpecies"), &needProcessorDir);
    testSpecies.updateAgent(testSpecies.createAgent(), new Time);
    Agent testAgent;
    testAgent.currentProcessChain = new ProcessChain;
    testAgent.currentProcessChain->push_back(new Process);
    (*testAgent.currentProcessChain)[0]->update(&testAgent, NULL, NULL); 
    return 1;
}
