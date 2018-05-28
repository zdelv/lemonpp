#pragma once
#ifndef EVENT_H
#define EVENT_H

#include "item.h"

// Main event type
// Contains the FG, BG colors and the message output by the item
// Used for emitting into the mainEventQueue
// Made of strings for the time being
// Its easier to push them to lemonbar if its a string 
// fgColor and bgColor are pushed in as simple ints, without the # or 0x
//      This is because of the requirement to remove the 0x
// TODO: Make this into a simplier data structure
//       Strings are kinda inefficient.
class event
{
    private:
        Item* itemPtr;
        std::string output;
    
    public:
        event(Item* _itemPtr, std::string _output)
            : itemPtr(_itemPtr), output(_output) {}

        ~event() 
        {
            std::cout << "Event from: " << itemPtr->getId() << " destroyed!" << std::endl;       
        }

        std::string createOutput();
        int getSenderId();

        // Utility function
        std::string getSenderMessage();
};

std::string intHextoString(int oldHex);

// Condition Variable and mutex used between the items and the eventWatchLoop
// This keeps the eventWatchLoop from pegging the CPU at 100% load
static std::condition_variable cv;
static std::mutex m;
static bool executing = true;

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

};
static eventQueue mainEventQueue;

#endif
