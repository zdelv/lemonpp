#ifndef USERITEMS_H
#define USERITEMS_H

#include "event.h"

// Generic Time item, might include some date ordering later
class Time: public Item
{
    private:
        std::time_t timeNow;
        std::tm* dateNow;
        char msg[100] ;

        void displayTime();
        void updateTime();

    public:
        Time(location _placement);

        std::string formatOutput();
        
        void timeEventLoop();
};

// Hooks on Xwindow
class Xwindow: public Item
{
    private:

    public:
        Xwindow(location _placement)
            : Item(_placement) {};
        ~Xwindow();
};

// Subscribes to the bspwm event queue and displays from it
class Bspwm: public Item
{
    private:

    public:
        Bspwm(location _placement);
        ~Bspwm();
};

// Battery Life
class Power: public Item
{
    private:

    public:
        Power();
        ~Power();
};
#endif
