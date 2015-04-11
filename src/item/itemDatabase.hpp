#ifndef ITEMDATABASE_HPP
#define ITEMDATABASE_HPP
#include <map>
#include <base2.0/ept/eptParser.hpp>
#include "item.hpp"

/* --ItemDatabase--
 * ItemDatabase holds information about items - what they can do, what
 * they are worth, etc. It's a basic lookup table loaded in from .ept specs
 *
 * It is meant to be initialized on startup. 
 * */
class ItemDatabase
{
    private:
        eptParser parser;
        std::string getAttribute(int type, int subType, const std::string& attribute);
    public:
        ItemDatabase();
        ~ItemDatabase();
        //Loads all scripts defined in itemDatabase.scripts to the private parser
        bool loadDatabase(const std::string& fileName);

        //Use these functions to get specific attributes for a specific item.
        //If the subtype ept definition does not contain the attribute, the
        //base definition will be searched. If that doesn't contain the attribute,
        //defaults will be returned (bools - false, int - 0, float - 0, string - "")
        bool getBoolAttribute(int type, int subType, const std::string& attribute);
        int getIntAttribute(int type, int subType, const std::string& attribute);
        float getFloatAttribute(int type, int subType, const std::string& attribute);
        //You can use string to get arrays - use eptParser.hpp conversion functions
        std::string getStringAttribute(int type, int subType, const std::string& attribute);
};
#endif
