#ifndef TIME_CPP
#define TIME_CPP
#include <cmath>
#include <iostream>
#include "time.hpp"

//const float MILLISECOND_TOLERANCE = 0.01;

Time::Time()
{
    years=0;
    days=0;
    seconds=0;
    milliseconds=0;
}
void Time::addMilliseconds(float msToAdd)
{
    milliseconds+=msToAdd;
    if (milliseconds > 1)
    {
        int secondsInMS = truncf(milliseconds);
        seconds+=(unsigned int) secondsInMS;
        milliseconds -= secondsInMS;
    }
}
void Time::getDeltaTime(Time* timeToCompare, Time& deltaTime)
{
    deltaTime.years = timeToCompare->years - years;
    deltaTime.days = timeToCompare->days - days;
    deltaTime.seconds = timeToCompare->seconds - seconds;
    deltaTime.milliseconds = timeToCompare->milliseconds - milliseconds;
}
bool Time::isGreaterThan(Time* timeToCompare)
{
    if (years < timeToCompare->years) return false;
    if (days < timeToCompare->days) return false;
    if (seconds + milliseconds < timeToCompare->seconds + timeToCompare->milliseconds)
    {
        //Account for very small differences in milliseconds
        //if ((seconds + milliseconds) - (timeToCompare->seconds + timeToCompare->milliseconds) >= MILLISECOND_TOLERANCE) return false;
        return false;
    }
    //if (milliseconds < timeToCompare->milliseconds) return false;
    return true;
}
void Time::invert()
{
    years = -years;
    days = -days;
    seconds = -seconds;
    milliseconds = -milliseconds;
}
void Time::print()
{
    std::cout << "Year " << years << ", Day " << days << ", Second " << seconds << ", ms " << milliseconds << "\n"; 
}
void Time::reset()
{
    years = 0;
    days = 0;
    seconds = 0;
    milliseconds = 0;
}
#endif
