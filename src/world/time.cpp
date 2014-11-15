#ifndef TIME_CPP
#define TIME_CPP
#include <cmath>
#include <iostream>
#include "time.hpp"

//const float MILLISECOND_TOLERANCE = 0.01;
const int DAYS_IN_YEAR = 365;
const int SECONDS_IN_DAY = 86400;

Time::Time()
{
    years=0;
    days=0;
    seconds=0;
    milliseconds=0;
}
int Time::getYears()
{
    return years;
}
int Time::getDays()
{
    return days;
}
int Time::getSeconds()
{
    return seconds;
}
float Time::getExactSeconds()
{
    return (float)seconds + milliseconds;
}
float Time::getMilliseconds()
{
    return milliseconds;
}

void Time::setYears(int newYears)
{
    years = newYears;
}
void Time::setDays(int newDays)
{
    if (newDays >= DAYS_IN_YEAR)
    {
        int numYears = newDays / DAYS_IN_YEAR;
        setYears(years + numYears);
        days += newDays % DAYS_IN_YEAR;
    }
    else
    {
        days = newDays;
    }
}
void Time::setSeconds(int newSeconds)
{
    if (newSeconds >= SECONDS_IN_DAY)
    {
        int numDays = newSeconds / SECONDS_IN_DAY;
        setDays(days + numDays);
        seconds += newSeconds % SECONDS_IN_DAY;
    }
    else
    {
        seconds = newSeconds;
    }
}
void Time::setMilliseconds(float newMilliseconds)
{
    if (fabs(newMilliseconds) > 1)
    {
        int secondsInMS = truncf(newMilliseconds);
        setSeconds((unsigned int) secondsInMS);
        milliseconds = newMilliseconds - secondsInMS;
    }
    else milliseconds = newMilliseconds;
}


void Time::scale(float scaleFactor, Time& result)
{
    result.setYears(years * scaleFactor);
    result.setDays(days * scaleFactor);
    result.setMilliseconds(getExactSeconds() * scaleFactor);
}
void Time::addSeconds(float secondsToAdd)
{
    setMilliseconds(secondsToAdd + getExactSeconds());
}
void Time::getDeltaTime(Time* timeToCompare, Time& deltaTime)
{
    deltaTime.setYears(timeToCompare->years - years);
    deltaTime.setDays(timeToCompare->days - days);
    deltaTime.setMilliseconds(timeToCompare->getExactSeconds() - getExactSeconds());
}
bool Time::isGreaterThan(Time* timeToCompare)
{
    if (years < timeToCompare->years) return false;
    if (days < timeToCompare->days) return false;
    if (getExactSeconds() < timeToCompare->getExactSeconds()) return false;
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
