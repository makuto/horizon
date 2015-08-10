#ifndef OBJECTPROCESSOR_CPP
#define OBJECTPROCESSOR_CPP
#include "objectProcessor.hpp"
#include "objectManager.hpp"
#include <base2.0/math/math.hpp>
#include <sstream>
#include "../world/eventManager.hpp"

ObjectProcessor::ObjectProcessor()
{
    in = NULL;
    processorType = -1;
    globalVecX = globalVecY = 0;
}
ObjectProcessor::~ObjectProcessor()
{
}
void ObjectProcessor::setup(InputState* newIn, PathManager* newPathManager, EventManager* newEvents)
{
    in = newIn;
    pathManager = newPathManager;
    events = newEvents;
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
    newObj->manhattanRadius = 25;
    newObj->target = -1;
    newObj->bounds.setPosition(position.getCellOffsetX() + newObj->boundOffsetX, position.getCellOffsetY() + newObj->boundOffsetY);
    if (newObj->subType==3) //Path test
    {
        Coord goal;
        goal.setPosition(1, 1, 1024, 1024);
        newObj->target = pathManager->requestNewPath(position, goal);
        //std::cout << "State: " << newObj->state << "\n";
        /*mainPath.init(manager->getWorld(), position, goal);
        mainPath.calculateCellPath();
        std::cout << "OBJ: Cell: " << mainPath.getStatus() << "\n";
        mainPath.calculateTilePath();
        std::cout << "OBJ: Tile: " << mainPath.getStatus() << "\n";*/
    }
    return true;
}
//Do a routine update on the object
int ObjectProcessor::updateObject(Object* obj, Time* globalTime, ObjectManager* manager)
{
    Coord prevPosition = obj->getPosition();
    Time delta;
    obj->lastUpdate.getDeltaTime(globalTime, delta);
    //std::cout << " delta: " << delta.getExactSeconds() << "\n";
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
        if (in->getInputState("movePlayerUp") > 0) vecY -= 2;
        if (in->getInputState("movePlayerDown") > 0) vecY += 2;
        if (in->getInputState("movePlayerLeft") > 0) vecX -= 2;
        if (in->getInputState("movePlayerRight") > 0) vecX += 2;

        //Attack
        if (in->getInputState("playerAttack") > 0)
        {
            if (obj->state <=0)
            {
                obj->state = 30; //Attack state
                Coord objPos = obj->getPosition();
                events->addEvent(EVENT_TYPE::ATTACK, objPos, globalTime, 10, obj);
            }
        }
        if (obj->state > 0)
        {
            float currentDelta = delta.getExactSeconds();
            if (currentDelta > 0.007)
            {
                while (currentDelta > 0.001)
                {
                    obj->state-=1; //Delay attack (cooldown)
                    currentDelta-=0.008;
                }
            }
        }
        
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
        Coord target;
        Coord objPos = obj->getPosition();
        switch(pathManager->advanceAlongPath((unsigned int)obj->target, objPos, target))
        {
            case 0: //Waiting for calculations
                //std::cout << "Waiting for calculation\n";
                break;
            case 1: //Ready
                obj->moveTowards(target, 300, &delta, *manager);
                break;
            case -1: //Path failed
                std::cout << "Path failed to calculate!\n";
                break;
            case -2: //Path doesn't exist
                std::cout << "Path doesn't exist! Creating new path\n";
                Coord goal;
                goal.setPosition(rand() % 10, rand() % 10, rand() % 2048, rand() % 2048);
                obj->target = pathManager->requestNewPath(objPos, goal);
                break;
            default: //Unknown return
                break;
        }
        /*switch(mainPath.getStatus())
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
        }*/
    }
    else if (obj->subType == 4) //Combat test
    {
        Coord objPos = obj->getPosition();
        
        //Handle events
        int sizeA;
        Event** currentEvents = events->getEventsInRangeCache(objPos, 30, sizeA);
        for (int i = 0; i < sizeA; i++)
        {
            Event* currentEvent = currentEvents[i];
            if (currentEvent->type==EVENT_TYPE::ATTACK)
            {
                Object* attacker = static_cast<Object*>(currentEvent->trigger);
                if (attacker)
                {
                    Coord attackerPos = attacker->getPosition();
                    CellIndex relIndex = objPos.getCell();
                    float attackerX = attackerPos.getRelativeCellX(relIndex);
                    float attackerY = attackerPos.getRelativeCellY(relIndex);
                    float objX = objPos.getRelativeCellX(relIndex);
                    float objY = objPos.getRelativeCellY(relIndex);
                    //Make sure attacker is facing this object
                    float facingDegrees = pointTowards(attackerX, attackerY, objX, objY) + 90;
                    const float FACING_TOLERANCE_DEGREES = 50;
                    if (fabs(facingDegrees - attacker->rotation) < FACING_TOLERANCE_DEGREES)
                    {
                        std::cout << "ATTACKED (time " << globalTime->getExactSeconds() << ") to face " << facingDegrees << " facing " << attacker->rotation << "\n";
                        obj->moveTowards(attackerPos, -600, &delta, *manager);
                    }
                }
            }
        }
        
        //Object* targetObj = manager->getWorld()->findObject(obj->target);
        aabb range(0, 0, 2048, 2048);
        int size;
        Object** nearObjs = manager->getObjectsInRangeCache(range, size);
        Object* targetObj = NULL;
        for (int i = 0; i < size; i++)
        {
            if (nearObjs[i]->id == obj->target)
            {
                targetObj = nearObjs[i];
                break;
            }
        }
        if (targetObj)
        {
            Coord objPos = obj->getPosition();
            Coord targetPos = targetObj->getPosition();
            float distance = objPos.getManhattanTo(targetPos);
            if (distance > 300) //Too far to chase; pathfind instead
            {
                obj->rotation++;
            }
            else if (distance <= 50)
            {
                //Attack
                if (obj->state <=0)
                {
                    float TURN_SPEED = 0.9;
                    CellIndex relIndex = objPos.getCell();
                    Coord attackerPos = targetPos;
                    //Rotate to face
                    float attackerX = attackerPos.getRelativeCellX(relIndex);
                    float attackerY = attackerPos.getRelativeCellY(relIndex);
                    float objX = objPos.getRelativeCellX(relIndex);
                    float objY = objPos.getRelativeCellY(relIndex);
                    float currentRotation = obj->rotation;
                    float newRotation = pointTowards(objX, objY, attackerX, attackerY) + 90; //Rotate image
                    float diff = newRotation - currentRotation;
                    if (diff > 180) diff = diff - 360;
                    else if (diff < -180) diff = diff + 360;
                    obj->rotation = (currentRotation + (diff * TURN_SPEED));
                    
                    obj->state = 30; //Attack state
                    Coord objPos = obj->getPosition();
                    events->addEvent(EVENT_TYPE::ATTACK, objPos, globalTime, 10, obj);
                }
                float currentDelta = delta.getExactSeconds();
                if (currentDelta > 0.007)
                {
                    while (currentDelta > 0.001)
                    {
                        obj->state-=1; //Delay attack (cooldown)
                        currentDelta-=0.008;
                    }
                }
            }
            else
            {
                obj->moveTowards(targetPos, 200, &delta, *manager);
            }
        }
    }
    CellIndex prevCell = prevPosition.getCell();
    obj->velX = obj->getPosition().getRelativeCellX(prevCell) - prevPosition.getCellOffsetX();
    obj->velY = obj->getPosition().getRelativeCellY(prevCell) - prevPosition.getCellOffsetY();
    float TURN_SPEED = 0.1;
    if ((obj->velX != 0 || obj->velY != 0) && obj->subType != 2) //Calculate facing direction
    {
        float currentRotation = obj->rotation;
        float newRotation = pointTowards(0, 0, obj->velX, obj->velY) + 90; //Rotate image
        float diff = newRotation - currentRotation;
        if (diff > 180) diff = diff - 360;
        else if (diff < -180) diff = diff + 360;
        //float oppDiff = 360 - diff;
        //if (oppDiff < diff) diff = oppDiff;
        //obj->rotation = (int)(currentRotation + (diff * TURN_SPEED)) % 360;
        obj->rotation = (currentRotation + (diff * TURN_SPEED));
    };
    obj->lastUpdate = *globalTime;
    return 1;
}
//Render the object (it is in view of player)
void ObjectProcessor::renderObject(Object* obj, float viewX, float viewY, window* win, RenderQueue* renderQueue)
{
    /*Coord pos = obj->getPosition();
    testSpr.setPosition(pos.getCellOffsetX() - viewX, pos.getCellOffsetY() - viewY);
    testSpr.setRotation(obj->rotation);
    win->draw(&testSpr);*/
    RenderInstance* instance = renderQueue->getInstance(RENDER_QUEUE_LAYER::ONGROUND, 0);
    if (instance) instance->position = obj->getPosition();
    instance->rotation=obj->rotation;
    if (obj->subType==2)
    {
        Coord objPos = obj->getPosition();
        float mouseX = in->getInputState("playerTargetX");
        float mouseY = in->getInputState("playerTargetY");
        //TODO: Normalize mouse input in InputState
        int objX = objPos.getCellOffsetX() - viewX;
        int objY = objPos.getCellOffsetY() - viewY;
        float winHalfWidth = win->getWidth() / 2;
        float winHalfHeight = win->getHeight() / 2;
        mouseX = (mouseX - winHalfWidth) / winHalfWidth;
        mouseY = (mouseY - winHalfHeight) / winHalfHeight;
        objX = (objX - winHalfWidth) / winHalfWidth;
        objY = (objY - winHalfHeight) / winHalfHeight;
        obj->rotation = pointTowards(objX, objY, mouseX, mouseY) + 90;
    }
    instance->imageId = "testCombatant";
    if (obj->state <= 20) instance->subRectId = "idle";
    else instance->subRectId = "attack";
    instance->requestorId = obj->id;
    instance->originMode = RENDER_ORIGIN_MODE::CENTER;
    instance->velX = obj->velX;
    instance->velY = obj->velY;
    /*//Debug bounds render
    sf::RenderWindow* sfWin = win->getBase();
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(obj->bounds.w, obj->bounds.h));
    rectangle.setFillColor(sf::Color::Transparent);
    if (obj->state > 500) rectangle.setOutlineColor(sf::Color::Red);
    else rectangle.setOutlineColor(sf::Color::Green);
    //obj->state--;
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
    return;*/
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
    if (collider->subType == 3)
    {
        touchedTile->x = 255;
        touchedTile->y = 255;
        return 0;
    }
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
    //collider->rotation++;
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
