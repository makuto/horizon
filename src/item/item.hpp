#ifndef ITEM_HPP
#define ITEM_HPP

/* --Item--
 * Items are things used by Objects. 
 * */
struct Item
{
    int type;   //Used by ItemManager to select this item's ItemProcessor
    int subType;    //Used by processors for specific items
    int id; //The ID of this particular item
    float state;  //Used by processors (for whatever is needed - esp time-related)
    int useState;   //The state of the item itself - is it held, in an inventory? etc.
    int stackCount; //The number of this item in this stack (if unstackable, 1)
};
#endif
