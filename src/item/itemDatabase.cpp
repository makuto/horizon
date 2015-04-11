#ifndef ITEMDATABASE_CPP
#define ITEMDATABASE_CPP
#include "itemDatabase.hpp"
#include <iostream>
//For building attribute paths
#include <sstream>
ItemDatabase::ItemDatabase()
{
}
ItemDatabase::~ItemDatabase()
{
}
bool ItemDatabase::loadDatabase(const std::string& fileName)
{
    if (!parser.load(fileName.c_str()))
    {
        std::cout << "ERROR: ItemDatabase.loadDatabase(): Failed to load master file list! Provided path:\n\t"
        << fileName << "\n";
        return false;
    }
    eptGroup* filesToLoad = parser.getGroup("itemDatabase.scripts");
    //Parse all files in files.testScripts
    std::string attrName;
    std::string currentFileName = filesToLoad->getAttributeFromIndex(0, attrName);
    for (int i = 1; currentFileName!=""; ++i)
    {
        if (!parser.load(currentFileName.c_str()))
        {
            std::cout << "ERROR: loadDatabase(): Failed to load script " << currentFileName << "!/n";
            return false;
        }
        //std::cout << "Loaded script " << currentFileName << " successfully\n";
        currentFileName = filesToLoad->getAttributeFromIndex(i, attrName);
    }
    return true;
}

std::string ItemDatabase::getAttribute(int type, int subType, const std::string& attribute)
{
    std::ostringstream ss;
    ss << type;
    eptFile* typeSpec = parser.getFile(ss.str());
    if (!typeSpec)
    {
        std::cout << "ERROR: itemDatabase.getAttribute(): Type " << type << " spec doesn't exist!\n";
        return "";
    }

    //Load the base specification
    eptGroup* baseSpec = typeSpec->getGroup("base");
    if (!baseSpec)
    {
        std::cout << "ERROR: itemDatabase.getAttribute(): Type " << type << " spec doesn't have base group defined\n";
        return "";
    }
    
    //Load the subtype specification
    std::ostringstream sT;
    sT << subType;
    eptGroup* subTypeSpec = typeSpec->getGroup(sT.str());
    if (!subTypeSpec)
    {
        std::cout << "WARNING: itemDatabase.getAttribute(): Type " << type << " subtype " << subType << " spec doesn't exist! Defaulting to base\n";
        subTypeSpec = baseSpec;
    }
    
    std::string attrValue = subTypeSpec->getAttribute(attribute);
    if (attrValue=="") 
    {
        //Attribute not found; search base
        attrValue = baseSpec->getAttribute(attribute);
    }
    return attrValue;
}
//Use these functions to get specific attributes for a specific item.
//If the subtype ept definition does not contain the attribute, the
//base definition will be searched. If that doesn't contain the attribute,
//defaults will be returned (bools - false, int - 0, float - 0, string - "")
bool ItemDatabase::getBoolAttribute(int type, int subType, const std::string& attribute)
{
    return attrToBool(getAttribute(type, subType, attribute));
}
int ItemDatabase::getIntAttribute(int type, int subType, const std::string& attribute)
{
    return attrToInt(getAttribute(type, subType, attribute));
}
float ItemDatabase::getFloatAttribute(int type, int subType, const std::string& attribute)
{
    return attrToFloat(getAttribute(type, subType, attribute));
}
//You can use string to get arrays - use eptParser.hpp conversion functions
std::string ItemDatabase::getStringAttribute(int type, int subType, const std::string& attribute)
{
    return getAttribute(type, subType, attribute);
}
#endif

