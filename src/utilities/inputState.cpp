#ifndef INPUTSTATE_CPP
#define INPUTSTATE_CPP
#include "inputState.hpp"
#include <iostream>
InputState::InputState()
{
    inputMan = NULL;
    lastUpdate.reset();
    inputSource = Source::INPUT_MANAGER; //By default
}
//Populates inputs map with the inputs specified in the spec;
//sets Source depending on the second parameter
void InputState::setup(eptGroup* spec, inputManager* newInputManager)
{
    //Prepare source
    inputMan = newInputManager;
    inputSource = Source::INPUT_MANAGER;

    //Prepare inputs map
    std::string attrName;
    std::string codeStr = spec->getAttributeFromIndex(0, attrName);
    for (int i = 1; codeStr != ""; i++)
    {
        Input newInput;
        newInput.name = attrName;
        newInput.code = attrToInt(codeStr);
        newInput.state = 0;
        inputs[attrName] = newInput;
        //Get next attribute
        codeStr = spec->getAttributeFromIndex(i, attrName);
    }
}

//Updates the states of all inputs in the inputs map
//Will eventually be able to read these from a file or network
void InputState::update(Time* globalTime)
{
    for (std::map<std::string, Input>::iterator it = inputs.begin();
    it != inputs.end(); ++it)
    {
        Input* currentInput = &it->second;
        //Update the state of the current input (depending on source)
        switch (inputSource)
        {
            case Source::INPUT_MANAGER:
                currentInput->state = inputMan->getState(currentInput->code);
                break;
            default:
                break;
        }
    }
    lastUpdate = *globalTime;
}

//Returns the current state of the input (since last update()) or
//0 if the input does not exist (which also causes an error to be printed)
int InputState::getInputState(const std::string& inputName)
{
    std::map<std::string, Input>::iterator findIt = inputs.find(inputName);
    if (findIt == inputs.end())
    {
        std::cout << "ERROR: InputState.getInputState(): Entry for input " << inputName << " doesn't exist!\n";
        return 0;
    }
    return findIt->second.state;
}
#endif

