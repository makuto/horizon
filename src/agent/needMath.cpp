#ifndef NEEDMATH_CPP
#define NEEDMATH_CPP
#include "needMath.hpp"
unsigned char addNoOverflow(unsigned char value, unsigned char amountToAdd)
{
    if (255 - amountToAdd > value) return value + amountToAdd;
    else return 255;
}
unsigned char subNoOverflow(unsigned char value, unsigned char amountToSub)
{
    if (amountToSub < value) return value - amountToSub;
    else return 0;
}
#endif
