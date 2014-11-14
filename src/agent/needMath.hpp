#ifndef NEEDMATH_HPP
#define NEEDMATH_HPP
//Use these functions to safely add and subtract from chars (prevents overflow)
unsigned char addNoOverflow(unsigned char value, unsigned char amountToAdd);
unsigned char subNoOverflow(unsigned char value, unsigned char amountToSub);
#endif

