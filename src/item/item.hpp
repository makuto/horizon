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
    int state;  //Used by processors (for whatever is needed)
    int useState;   //The state of the item itself - is it held, in an inventory? etc.
};
#endif
