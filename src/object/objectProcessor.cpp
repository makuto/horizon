#ifndef OBJECTPROCESSOR_CPP
#define OBJECTPROCESSOR_CPP
#include "objectProcessor.hpp"
#include "objectManager.hpp"
#include <sstream>
ObjectProcessor::ObjectProcessor()
{
    processorType = -1;
}
ObjectProcessor::~ObjectProcessor()
{
}
void ObjectProcessor::setup(inputManager* newIn)
{
    in = newIn;
}
int ObjectProcessor::getType()
{
    return processorType;
}
void ObjectProcessor::initialize(eptFile* spec)
{
    processorType = attrToInt(spec->getAttribute("defaults.type"));
    testSpr.load(spec->getAttribute("renderSettings.image").c_str());
    testSpr.setOrigin(16, 16);
    return;
}
//--Overload functions for object dynamic behavior
//--Functions should return -1 if the object should be destroyed
//Called to initialize a new-ed object - use this function to make sure
//objects are valid subtypes in valid positions
//Return false if the object cannot be created
bool ObjectProcessor::initObject(Object* newObj, int subType, Coord& position, float rotation, ObjectManager* manager)
{
    newObj->subType = subType;
    newObj->setPosition(position, *manager);
    newObj->rotation = rotation;
    newObj->lastUpdate.reset();
    newObj->bounds.w = 32;
    newObj->bounds.h = 32;
    newObj->boundOffsetX = -16;
    newObj->boundOffsetY = -16;
    newObj->manhattanRadius = 1000;
    newObj->bounds.setPosition(position.getCellOffsetX() + newObj->boundOffsetX, position.getCellOffsetY() + newObj->boundOffsetY);
    return true;
}
//Do a routine update on the object
int ObjectProcessor::updateObject(Object* obj, Time* globalTime, ObjectManager* manager)
{
    Time delta;
    obj->lastUpdate.getDeltaTime(globalTime, delta);
    //obj->rotation += delta.getExactSeconds() * 200;
    float speed = 100; //Why doesn't 25 work?
    //float vecX = obj->getPosition().getTrueX() + 512;
    //float vecY = obj->getPosition().getTrueY() + 512;
    float vecX = 0;
    float vecY = 0;
    if (obj->subType == 1)
    {
        if (obj->getPosition().getTrueX() !=512)
        {
            vecX = -obj->getPosition().getTrueX() + 512;
        }
        if (obj->getPosition().getTrueY() !=512)
        {
            vecY = -obj->getPosition().getTrueY() + 512;
        }
        
        //Normalize
        float dist = sqrtf((vecX * vecX) + (vecY * vecY));
        if (dist > 1)
        {
            vecX /= dist;
            vecY /= dist;
            vecX *= speed;
            vecY *= speed;
            vecX *= delta.getExactSeconds();
            vecY *= delta.getExactSeconds();
            //if (obj->state != 1)
            //{
            if (obj->id == 53014) std::cout << vecX << " , " << vecY << " processor\n";
                obj->addVector(vecX, vecY, *manager);
            //}
        }
    }
    else if (obj->subType == 2)
    {
        if (in->isPressed(inputCode::W)) vecY -= 2;
        if (in->isPressed(inputCode::S)) vecY += 2;
        if (in->isPressed(inputCode::A)) vecX -= 2;
        if (in->isPressed(inputCode::D)) vecX += 2;
        obj->addVector(vecX, vecY, *manager);
    }
    obj->lastUpdate = *globalTime;
    return 1;
}
//Render the object (it is in view of player)
void ObjectProcessor::renderObject(Object* obj, float viewX, float viewY, window* win)
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
    ss << obj->id;
    text textToRender;
    textToRender.setSize(14);
    textToRender.setColor(255, 38, 38, 255);
    if (!textToRender.loadFont("data/fonts/font1.ttf"))
    {
        std::cout << "err: cannot load debugText font\n";
        return;
    }
    textToRender.setPosition(obj->getPosition().getTrueX() - viewX + 16, obj->getPosition().getTrueY() - viewY + 16);
    textToRender.setText(ss.str());
    win->draw(&textToRender);
    return;
}
//Agent uses/activates object
int ObjectProcessor::activateObject(Object* obj, Agent* agent)
{
    return 1;
}

//Agent collides with object
int ObjectProcessor::touchObject(Object* collider, Coord& collideDisplacement, Agent* agent, bool isMoving)
{
    return 1;
}
//Object collides with tile
int ObjectProcessor::onCollideTile(Object* collider, Coord& collideDisplacement, tile& touchedTile, bool isMoving)
{
    return 1;
}
//Object collides with object
int ObjectProcessor::onCollideObj(Object* collider, Coord& collideDisplacement, Object* obj, bool isMoving)
{
    //collider->state = 501;
    /*if (collider->state != 1 && isMoving)
    {
        collider->state = 1;
        return 1;
    }*/
    /*if (obj->subType==2 && !isMoving)
    {
        CellIndex currentCell = collideDisplacement.getCell();
        collideDisplacement.setPosition(currentCell, 32, 32);
        return 0;
    }*/
    collider->rotation++;
    return 1;
}

//Object is hit by something
//virtual int hitObject(Object* obj, Actor* actor, int inventoryIndex); //TODO
//This function is called when an object is going to be destroyed
//Use this function to do any cleanup if needed. The Object itself
//should NOT be deleted by this function
void ObjectProcessor::onDestroyObject(Object* obj)
{
    return;
}
#endif
