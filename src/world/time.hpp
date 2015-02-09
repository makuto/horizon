#ifndef TIME_HPP
#define TIME_HPP

extern const int DAYS_IN_YEAR;
extern const int SECONDS_IN_DAY;

class Time
{
    protected:
        int years;
        int days;
        int seconds;
        float milliseconds;
    public:
        Time();
        
        int getYears();
        int getDays();
        int getSeconds();
        float getExactSeconds();
        float getMilliseconds();

        void setYears(int newYears);
        void setDays(int newDays);
        void setSeconds(int newSeconds);
        void setMilliseconds(float newMilliseconds);

        void scale(float scaleFactor, Time& result);
        //Adds an exact second, making sure to account for overflows
        void addSeconds(float secondsToAdd);
        //Gets the delta between this time and timeToCompare and
        //stores the result in deltaTime; same as timeToCompare - this time
        void getDeltaTime(Time* timeToCompare, Time& deltaTime);
        bool isGreaterThan(Time* timeToCompare);
        void invert();
        void print();
        void reset();
};
#endif
