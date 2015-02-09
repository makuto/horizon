#ifndef AGENTPROCESSOR_CPP
#define AGENTPROCESSOR_CPP
#include <sstream>
#include "agentProcessor.hpp"
#include "../objectManager.hpp"

AgentProcessor::AgentProcessor(Species* newSpecies, ProcessDirectory* newProcessDir)
{
    species = newSpecies;
    processDir = newProcessDir;
}
AgentProcessor::~AgentProcessor()
{
    
}
//Use this function to init your custom ObjectProcessor
bool AgentProcessor::initialize(eptFile* spec)
{
    processorType = attrToInt(spec->getAttribute("defaults.type"));
    if (!testSpr.load(spec->getAttribute("renderSettings.image").c_str())) return false;
    testSpr.setOrigin(16, 16);
    return true;
}
//--Overload functions for object dynamic behavior
//--Functions should return -1 if the object should be destroyed
//Called to initialize a new-ed object - use this function to make sure
//objects are valid subtypes in valid positions.
//Return false if the object cannot be created
bool AgentProcessor::initObject(Object* newObj, int subType, Coord& position, float rotation, ObjectManager* manager)
{
    newObj->subType = subType;
    newObj->setPosition(position, *manager);
    newObj->rotation = rotation;
    newObj->lastUpdate.reset();
    newObj->bounds.w = 30;
    newObj->bounds.h = 30;
    newObj->boundOffsetX = -15;
    newObj->boundOffsetY = -15;
    newObj->manhattanRadius = 1000;
    newObj->bounds.setPosition(position.getCellOffsetX() + newObj->boundOffsetX, position.getCellOffsetY() + newObj->boundOffsetY);
    return true;
}
//Do a routine update on the object; set obj->lastUpdate to globalTime
//once finished (used to get the delta)
int AgentProcessor::updateObject(Object* obj, Time* globalTime, ObjectManager* manager)
{
    //Subtype is used to store the agentID
    Agent* agent = species->getAgent(obj->subType);
    if (!agent)
    {
        std::cout << "ERROR: AgentProcessor::updateObject(): Agent with ID " << obj->subType << " doesn't exist!\n";
        return -1;
    }
    Time deltaTime;
    obj->lastUpdate.getDeltaTime(globalTime, deltaTime);
    species->updateAgent(agent, obj, manager, globalTime, &deltaTime, processDir);
    
    obj->lastUpdate = *globalTime;
    return 1;
}
//Render the object (it is in view). ViewX and Y are the window's top left corner coordinates
//relative to the current cell.
void AgentProcessor::renderObject(Object* obj, float viewX, float viewY, window* win)
{
    Coord pos = obj->getPosition();
    testSpr.setPosition(pos.getCellOffsetX() - viewX, pos.getCellOffsetY() - viewY);
    testSpr.setRotation(obj->rotation);
    win->draw(&testSpr);
    //Debug bounds render
    sf::RenderWindow* sfWin = win->getBase();
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(obj->bounds.w, obj->bounds.h));
    rectangle.setFillColor(sf::Color::Transparent);
    if (obj->state > 500) rectangle.setOutlineColor(sf::Color::Red);
    else rectangle.setOutlineColor(sf::Color::Green);
    obj->state--;
    rectangle.setOutlineThickness(2);
    rectangle.setPosition(obj->getPosition().getCellOffsetX() - viewX + obj->boundOffsetX, obj->getPosition().getCellOffsetY() - viewY + obj->boundOffsetY);
    sfWin->draw(rectangle);
    rectangle.setOutlineColor(sf::Color::Green);
    rectangle.setSize(sf::Vector2f(2, 2));
    rectangle.setPosition(obj->getPosition().getCellOffsetX() - viewX, obj->getPosition().getCellOffsetY() - viewY);
    sfWin->draw(rectangle);
    rectangle.setOutlineColor(sf::Color::White);
    rectangle.setPosition(obj->getPosition().getTrueX() - viewX, obj->getPosition().getTrueY() - viewY);
    sfWin->draw(rectangle);
    
    std::stringstream ss;
    ss << obj->id << "\n[ " << pos.getCell().x << " , " << pos.getCell().y << " ]"
    << "\n( " << pos.getTrueX() << " , " << pos.getTrueY() << " )" ;
    text textToRender;
    textToRender.setSize(14);
    textToRender.setColor(255, 38, 38, 255);
    if (!textToRender.loadFont("data/fonts/font1.ttf"))
    {
        std::cout << "err: cannot load debugText font\n";
        return;
    }
    textToRender.setPosition(obj->getPosition().getCellOffsetX() - viewX + 16, obj->getPosition().getCellOffsetY() - viewY + 16);
    textToRender.setText(ss.str());
    win->draw(&textToRender);
}
//Agent uses/activates object
/*int AgentProcessor::activateObject(Object* obj, Agent* agent)
{
    return 1;
}*/
//Object is hit by a used item (sword etc)
//virtual int hitObject(Object* obj, Actor* actor, int inventoryIndex); //TODO

//Collision functions take the collider, the coordinate that the collider
//is trying to reach (but collides with something), and the object/agent/tile
//that the object is colliding with. Additionally, isMoving is passed that
//indicates if this object is moving into others if true, or if objects
//are moving into this object if false (if false, collideDisplacement is the
//second object's, NOT the collider's)
//By returning different codes, objects can control how they want the collision resolved:
//(note that isMoving true objects take precedence)
//0 - The ObjectManager should not try to resolve the collision
//    (the collision is assumed to be resolved in the object's own
//    collision functions)
//1 - The ObjectManager should try to resolve the collision. if
//    isMoving is false, this means the stationary object will stay still
//    and the moving object will be changed
//2 - The ObjectManager should destroy the object
//3 - The ObjectManager should move the object to the colliding
//    coordinate and move everything else in the way (obviously, this
//    doesn't work with tiles). If isMoving is false, this means
//    the stationary (nonmoving) object will be moved by the moving object
//Agent collides with object
/*int AgentProcessor::touchObject(Object* collider, Coord& collideDisplacement, Agent* agent, bool isMoving)
{
    return 1;
}*/
//Object collides with tile
int AgentProcessor::onCollideTile(Object* collider, Coord& collideDisplacement, tile& touchedTile, bool isMoving)
{
    return 1;
}
//Object collides with object
int AgentProcessor::onCollideObj(Object* collider, Coord& collideDisplacement, Object* obj, bool isMoving)
{
    return 1;
}

//This function is called when an object is going to be destroyed
//Use this function to do any cleanup if needed. The Object itself
//should NOT be deleted by this function
void AgentProcessor::onDestroyObject(Object* obj)
{
    species->removeAgent(obj->subType);
}

#endif
