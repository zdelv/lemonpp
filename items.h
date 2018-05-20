#pragma once
#ifndef LEMON_H
#define LEMON_H

#include <vector>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread> 
#include <memory>
#include <queue>
#include <string>
#include <condition_variable>
#include <mutex>
#include <cstdlib>

// Main event type
// Contains the FG, BG colors and the message output by the item
// Used for emitting into the eventFifo
// Made of strings for the time being
// Its easier to push them to lemonbar if its a string 
// TODO: Make sender automatically pull either id or something else
struct event
{
    std::string fgColor, bgColor, sender, message;

    event(std::string _fgColor, std::string _bgColor, std::string _sender, std::string _message)
        : fgColor(_fgColor), bgColor(_bgColor), sender(_sender), message(_message) {}
};

// Condition Variable and mutex used between the items and the eventWatchLoop
// This keeps the eventWatchLoop from pegging the CPU at 100% load
std::condition_variable cv;
std::mutex m;
bool executing = true;

int highestId = 0;

static std::queue<event> eventFifo;

using namespace std::chrono_literals;
enum class location : char
{
    left,
    right,
    center
};

// Global Function for executing bash scripts/programs and
// piping output into an std::string for processing
// From: https://stackoverflow.com/q/478898
// TODO?: Might change this to later push out a shared_ptr rather than a string
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);

    if (!pipe) 
        throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) 
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }

    return result;
}

class Item
{
    private:
        const location placement;
        const int id;

    protected:
        std::thread eventThread;
        int itemAlive;

        // Pushes an event into the bar FIFO
        void itemPushEvent(event input);

    public:
        Item(location _placement)
            : placement(_placement), id(highestId + 1) 
        {
            highestId += 1;
        };

        Item(Item&& that) noexcept
            : placement(that.placement), id(that.id), eventThread(std::move(that.eventThread))
        {
        
        };

        Item& operator=(Item&&) = default;

        /* Item(const Item& item) */
        /*     : placement(item.placement), id(highestId + 1) */
        /* { */
        /*     highestId += 1; */
        /* } */
        ~Item() { eventThread.join(); };

        int getId() { return id; };
        operator std::string() { return std::to_string(id); };
        void kill() { itemAlive = false; };
};

// Generic Time item, might include some date ordering later
class Time: public Item
{
    private:
        std::time_t timeNow;
        std::tm* dateNow;
        void displayTime();
        void updateTime();

    public:
        Time(location _placement);
        
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

void Item::itemPushEvent(event input)
{
    eventFifo.push(input);   
}

Time::Time(location _placement)
    : Item(_placement)
{
    timeNow = std::time(0);
    dateNow = std::localtime(&timeNow);
    eventThread = std::thread(&Time::timeEventLoop, this);
}

// TODO: Switch to std::strftime() for custom output
void Time::displayTime()
{
    std::cout << std::ctime(&timeNow) << " : " << getId() << std::endl;  
}

void Time::updateTime()
{
    timeNow = std::time(0); 
}

// Main loop for the Time thread
// Changes the updates the time then signals CV
void Time::timeEventLoop()
{
    while (itemAlive) 
    {
        std::cout << "Time thread start" << std::endl;
        updateTime();

        // TODO: Make this a shared_ptr and continually update it 
        // rather than make and destroy new variables
        event time("black", "white", "Time", std::ctime(&timeNow));
        itemPushEvent(time);
        std::cout << "Pushed" << std::endl;

        cv.notify_one();
        std::cout << "Time thread notifies" << std::endl;

        std::this_thread::sleep_for(2s);
    }
}
#endif
