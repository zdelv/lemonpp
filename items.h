#pragma once
#ifndef ITEMS_H
#define ITEMS_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread> 
#include <memory>
#include <queue>
#include <sstream>
#include <condition_variable>
#include <mutex>
#include <cstdlib>

using namespace std::chrono_literals;
//TODO: Make all threads into pointers to stop shells from forming after moves

// Main event type
// Contains the FG, BG colors and the message output by the item
// Used for emitting into the mainEventQueue
// Made of strings for the time being
// Its easier to push them to lemonbar if its a string 
// fgColor and bgColor are pushed in as simple ints, without the # or 0x
//      This is because of the requirement to remove the 0x
// TODO: Make this into a simplier data structure
//       Strings are kinda inefficient.
struct event
{
    std::string placement, senderId, message;
    int fgColor, bgColor;

    event(int _fgColor, int _bgColor, std::string _placement, std::string _senderId, std::string _message)
        : fgColor(_fgColor), bgColor(_bgColor), placement(_placement), senderId(_senderId), message(_message) {}
    ~event() 
    {
        std::cout << "Event from: " << senderId << " destroyed!" << std::endl;       
    }
};

std::string intHextoString(int oldHex)
{
    return (std::to_string(oldHex)).erase(0,1);
}

// Condition Variable and mutex used between the items and the eventWatchLoop
// This keeps the eventWatchLoop from pegging the CPU at 100% load
std::condition_variable cv;
std::mutex m;
bool executing = true;

// Class built around an mainEventQueue, to allow for correct blocking of threads
// This is necessary even though its a multi-producer, single consumer system
// Without this, the threads push items on top of each other
// mainEventQueue is defined right on this class declaration.
class eventQueue
{
    private:
        std::queue<event> eventFifo;

        std::mutex queueMutex;

    public:
        eventQueue() {};

        ~eventQueue() {};

        // Concurrent blocking is done on these functions
        // This keeps the threads from accessing the queue simultaneously
        void push(const event& newEvent);
        void pop();
        event front();
        bool empty();

} mainEventQueue;

// This pushes an event onto the queue
// It must own the queueMutex
void eventQueue::push(const event& newEvent)
{
    // Scoped to kill the lock_guard
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        eventFifo.push(newEvent);
    }
    cv.notify_all();
}

void eventQueue::pop()
{
    // Scoped to kill the lock_guard
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        eventFifo.pop();
    }
}

event eventQueue::front()
{
    std::lock_guard<std::mutex> lock(queueMutex);
    return eventFifo.front();
}

bool eventQueue::empty()
{
    std::lock_guard<std::mutex> lock(queueMutex);
    return eventFifo.empty();
}



enum class location : char
{
    left='l',
    right='r',
    center='c'
};

std::string locToString(location input)
{
    std::string outputLoc; 
    switch(input)
    {
        case(location::left):   outputLoc = "l"; break;
        case(location::right):  outputLoc = "r"; break;
        case(location::center): outputLoc = "c"; break;
        default: std::cout << "Invalid location used!" << std::endl; break;
    }
    return outputLoc;
}

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

        static int itemCount;

    protected:
        std::thread eventThread;
        int itemAlive;

        // Pushes an event into the bar FIFO
        void itemPushEvent(event input);

        // Contains the last output sent to be rendered
        std::string lastOutput;

    public:
        Item(location _placement)
            : placement(_placement), id(itemCount) 
        {
            // TODO: Make this work with a static itemCount and const id
            ++itemCount;
        };

        // Move constructor
        // This should just move the placaement and id correctly, then move the thread.
        // TODO: Probably kill the old item. It is just a shell, since the thread was moved
        Item(Item&& that) noexcept
            : placement(that.placement), id(that.id), eventThread(std::move(that.eventThread)) {};

        Item& operator=(Item&&) = default;

        /* Item(const Item& item) */
        /*     : placement(item.placement), id(highestId + 1) */
        /* { */
        /*     highestId += 1; */
        /* } */

        // TODO: Flush this out more
        ~Item() { eventThread.join(); };

        int getId() { return id; };

        // This might not be needed anymore
        operator std::string() { return std::to_string(id); };

        // TODO: Make this actually work as a global kill function
        void kill() { itemAlive = false; };
        location getPlacement();

        virtual std::string formatOutput() = 0;
};

int Item::itemCount = 0;

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

Time::Time(location _placement)
    : Item(_placement)
{
    timeNow = std::time(0);
    dateNow = std::localtime(&timeNow);
    eventThread = std::thread(&Time::timeEventLoop, this);
}

void Item::itemPushEvent(event input)
{
    mainEventQueue.push(input);   
}

location Item::getPlacement()
{
    return this->placement;     
}

// TODO: Switch to std::strftime() for custom output
// This is useless probably, maybe
void Time::displayTime()
{
    std::cout << std::ctime(&timeNow) << " : " << getId() << std::endl;  
}

void Time::updateTime()
{
    timeNow = std::time(0); 
    dateNow = std::localtime(&timeNow);
}

std::string Time::formatOutput()
{
    std::unique_ptr<std::stringstream> timeStream = std::make_unique<std::stringstream>();

    *timeStream << "%{F" << intHextoString(0xff00ff) << "}%{B" << "#00ff00" \
                    << "} " << this->msg << " %{B-}%{F-}";

    lastOutput = timeStream->str();
    return timeStream->str();
}

// Main loop for the Time thread
// Changes the updates the time then signals CV
void Time::timeEventLoop()
{
    while (itemAlive) 
    {
        /* std::cout << "Time thread start" << std::endl; */
        this->updateTime();

        std::strftime(this->msg, sizeof(this->msg), "%a %R", dateNow);

        std::string output = this->formatOutput();
        event time(0xff0000, 0x00ff00, locToString(getPlacement()), std::to_string(this->getId()), output);

        this->itemPushEvent(time);
        /* std::cout << "Pushed" << std::endl; */

        cv.notify_one();
        /* std::cout << "Time thread notifies" << std::endl; */

        std::this_thread::sleep_for(2s);
    }
}
#endif
