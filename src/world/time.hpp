#ifndef TIME_HPP
#define TIME_HPP
class Time
{
    public:
        int years;
        int days;
        int seconds;
        float milliseconds;

        Time();
        void addMilliseconds(float msToAdd);
        void getDeltaTime(Time* timeToCompare, Time& deltaTime);
        bool isGreaterThan(Time* timeToCompare);
        void invert();
        void print();
        void reset();
};
#endif
