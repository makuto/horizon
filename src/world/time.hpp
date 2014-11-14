#ifndef TIME_HPP
#define TIME_HPP
class Time
{
    public:
        unsigned int years;
        unsigned int days;
        unsigned int seconds;
        float milliseconds;

        Time();
        void addMilliseconds(float msToAdd);
        void getDeltaTime(Time* timeToCompare, Time& deltaTime);
        void invert();
        void print();
        void reset();
};
#endif
