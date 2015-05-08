#ifndef IMAGEMANAGER_CPP
#define IMAGEMANAGER_CPP
#include "imageManager.hpp"
#include <iostream>
ImageManager::ImageManager()
{
    
}
ImageManager::~ImageManager()
{
    
}

bool ImageManager::load(eptFile* spec)
{
    //Loop through all groups (representing images), loading images and subrects as we go
    std::string imageHandle;
    eptGroup* imageGroup = spec->getGroupFromIndex(0, imageHandle);
    for (int i = 1; imageGroup != NULL; i++)
    {
        //Load the sprite
        sprite* newSprite = new sprite();
        if (!newSprite->load(imageGroup->getAttribute("_file").c_str())) return false;
        sprites[imageHandle] = newSprite;
        std::cout << "Loaded image " << imageGroup->getAttribute("_file") << " successfully\n";
        
        //Load the subrects
        std::string subRectHandle;
        std::string subRectStr = imageGroup->getAttributeFromIndex(0, subRectHandle);
        for (int n = 1; subRectStr != ""; n++)
        {
            //Attributes preceded by _ are not subRects (ignore them)
            if (subRectHandle[0] != '_')
            {
                SubRect newSubRect;
                //Get the subrect from the attr array ( {x, y, w, h} )
                newSubRect.x = attrToArrayInt(subRectStr, 0);
                newSubRect.y = attrToArrayInt(subRectStr, 1);
                newSubRect.width = attrToArrayInt(subRectStr, 2);
                newSubRect.height = attrToArrayInt(subRectStr, 3);
                subRects[subRectHandle] = newSubRect;
            }
            //Get the next attribute
            subRectStr = imageGroup->getAttributeFromIndex(n, subRectHandle);
        }
        
        //Get the next group
        imageGroup = spec->getGroupFromIndex(i, imageHandle);
    }
    return true;
}
//Returns the sprite with the specified handle, or NULL if it doesn't exist
sprite* ImageManager::getSprite(const std::string& spriteName)
{
    std::map<std::string, sprite*>::iterator findIt = sprites.find(spriteName);
    if (findIt==sprites.end()) return NULL;
    sprite* spr = findIt->second;
    //Reset rotation
    spr->setRotation(0);
    //Reset subrect
    spr->clearSubRect();
    return spr;
}
//Returns the sprite with the desired subRect applied, or NULL if either
//do not exist
sprite* ImageManager::getSubRectSprite(const std::string& spriteName, const std::string& subRectName)
{
    //Grab the sprite
    sprite* spr = getSprite(spriteName);
    if (!spr) return NULL;
    //Grab the subrect
    std::map<std::string, SubRect>::iterator findIt = subRects.find(subRectName);
    if (findIt == subRects.end()) return NULL;
    SubRect* rect = &findIt->second;
    //Set the sprite subrect to the requested subRect
    spr->setSubRect(rect->x, rect->y, rect->width, rect->height);
    return spr;
}
//Returns the sprite with the desired subRect applied, or NULL if either
//do not exist
sprite* ImageManager::getSubRectSprite(const std::string& spriteName, const std::string& subRectName, SubRect& subRect)
{
    //Grab the sprite
    sprite* spr = getSprite(spriteName);
    if (!spr) return NULL;
    //Grab the subrect
    std::map<std::string, SubRect>::iterator findIt = subRects.find(subRectName);
    if (findIt == subRects.end()) return NULL;
    SubRect* rect = &findIt->second;
    //Set the sprite subrect to the requested subRect
    spr->setSubRect(rect->x, rect->y, rect->width, rect->height);
    //Pass the subRect pointer to the provided one (they want it)
    subRect = *rect;
    return spr;
}
#endif
