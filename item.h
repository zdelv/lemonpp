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

class event;

enum class location : char
{
    left='l',
    right='r',
    center='c'
};

using namespace std::chrono_literals;

std::string locToString(location input);

std::string exec(const char* cmd);

// Global Function for executing bash scripts/programs and
// piping output into an std::string for processing
// From: https://stackoverflow.com/q/478898
// TODO?: Might change this to later push out a shared_ptr rather than a string

class Item : public std::enable_shared_from_this<Item>
{
    private:
        const int id;
        
        const location placement;
        int fgColor, bgColor;
        std::string message;

        // This should default init to 0
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


        // This might not be needed anymore
        operator std::string() { return std::to_string(id); };

        // TODO: Make this actually work as a global kill function
        void kill() { itemAlive = false; };
        location getPlacement();

        int getId() { return id; };
        std::string getMessage() { return message; };
        virtual std::string formatOutput() = 0;
};
#endif
