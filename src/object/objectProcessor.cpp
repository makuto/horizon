#ifndef OBJECTPROCESSOR_CPP
#define OBJECTPROCESSOR_CPP
#include "objectProcessor.hpp"
#include "objectManager.hpp"
#include <base2.0/math/math.hpp>
#include <sstream>
ObjectProcessor::ObjectProcessor()
{
    in = NULL;
    processorType = -1;
    globalVecX = globalVecY = 0;
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
bool ObjectProcessor::initialize(eptFile* spec)
{
    processorType = attrToInt(spec->getAttribute("defaults.type"));
    if (!testSpr.load(spec->getAttribute("renderSettings.image").c_str())) return false;
    testSpr.setOrigin(16, 16);
    return true;
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
    newObj->bounds.w = 30;
    newObj->bounds.h = 30;
    newObj->boundOffsetX = -15;
    newObj->boundOffsetY = -15;
    newObj->manhattanRadius = 1000;
    newObj->bounds.setPosition(position.getCellOffsetX() + newObj->boundOffsetX, position.getCellOffsetY() + newObj->boundOffsetY);
    if (newObj->subType==3) //Path test
    {
        Coord goal;
        goal.setPosition(1, 1, 1024, 1024);
        mainPath.init(manager->getWorld(), position, goal);
        mainPath.calculateCellPath();
        std::cout << "OBJ: Cell: " << mainPath.getStatus() << "\n";
        mainPath.calculateTilePath();
        std::cout << "OBJ: Tile: " << mainPath.getStatus() << "\n";
    }
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
        float targetX = 0;
        float targetY = 0;
        if (obj->getPosition().getTrueX() !=targetX)
        {
            vecX = -obj->getPosition().getTrueX() + targetX;
        }
        if (obj->getPosition().getTrueY() !=targetY)
        {
            vecY = -obj->getPosition().getTrueY() + targetY;
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
            globalVecX = vecX;
            globalVecY = vecY;
            if (globalTime->getExactSeconds() < 30)
            {
                obj->addVector(vecX, vecY, *manager);
            }
        }
    }
    else if (obj->subType == 2) //Human input test
    {
        if (in->isPressed(inputCode::W)) vecY -= 2;
        if (in->isPressed(inputCode::S)) vecY += 2;
        if (in->isPressed(inputCode::A)) vecX -= 2;
        if (in->isPressed(inputCode::D)) vecX += 2;
        coordinate vec;
        vec.x = vecX;
        vec.y = vecY;
        normalize(&vec);
        /*float mouseX = in->getMouseX();
        float mouseY = in->getMouseY();
        float objX = obj->getPosition().getCellOffsetX();
        float objY = obj->getPosition().getCellOffsetY();
        vecX += mouseX - objX;
        vecY += mouseY - objY;*/
        speed = 300;
        vecX = vec.x * delta.getExactSeconds() * speed;
        vecY = vec.y * delta.getExactSeconds() * speed;
        globalVecX = vecX;
        globalVecY = vecY;
        //std::cout << vecX << " , " << vecY << "\n";
        obj->addVector(vecX, vecY, *manager);
    }
    else if (obj->subType == 3) //Follow path test
    {
        switch(mainPath.getStatus())
        {
            case 0:
                mainPath.calculateTilePath();
                break;
            case 1:
                Coord target;
                Coord objPos;
                objPos = obj->getPosition();
                target = mainPath.advance(objPos);
                target.print();
                CellIndex relCell;
                relCell = obj->getPosition().getCell();
                float axDiff;
                axDiff = obj->getPosition().getRelativeCellX(relCell) - target.getRelativeCellX(relCell);
                float ayDiff;
                ayDiff = obj->getPosition().getRelativeCellY(relCell) - target.getRelativeCellY(relCell);
                coordinate vec;
                vec.x = axDiff;
                vec.y = ayDiff;
                normalize(&vec);
                float speed;
                speed = 300;
                float vecX;
                vecX = -vec.x * delta.getExactSeconds() * speed;
                float vecY;
                vecY = -vec.y * delta.getExactSeconds() * speed;
                globalVecX = vecX;
                globalVecY = vecY;
                std::cout << "VEC " << vecX << " , " << vecY << "\n";
                obj->addVector(vecX, vecY, *manager);
                break;
            default:
                break;
        }
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
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setPosition(obj->getPosition().getCellOffsetX() - viewX + globalVecX, obj->getPosition().getCellOffsetY() - viewY + globalVecY);
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

    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(obj->getPosition().getCellOffsetX() - viewX + 15, obj->getPosition().getCellOffsetY() - viewY + 15)),
        sf::Vertex(sf::Vector2f(obj->getPosition().getCellOffsetX() - viewX + 15 + globalVecX, obj->getPosition().getCellOffsetY() - viewY + 15 + globalVecY))
    };

    win->getBase()->draw(line, 2, sf::Lines);
    return;
}
/*//Agent uses/activates object
int ObjectProcessor::activateObject(Object* obj, Agent* agent)
{
    return 1;
}

//Agent collides with object
int ObjectProcessor::touchObject(Object* collider, Coord& collideDisplacement, Agent* agent, bool isMoving)
{
    return 1;
}*/
//Object collides with tile
int ObjectProcessor::onCollideTile(Object* collider, Coord& collideDisplacement, tile* touchedTile)
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
