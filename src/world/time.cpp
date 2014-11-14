#ifndef TIME_CPP
#define TIME_CPP
#include <cmath>
#include <iostream>
#include "time.hpp"
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
    deltaTime.years = years - timeToCompare->years;
    deltaTime.days = days - timeToCompare->days;
    deltaTime.seconds = seconds - timeToCompare->seconds;
    deltaTime.milliseconds = milliseconds - timeToCompare->milliseconds;
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
