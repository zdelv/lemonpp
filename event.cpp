#include "event.h"

// TODO: Move this to a utility file or something
std::string intHextoString(int oldHex)
{
    return (std::to_string(oldHex)).erase(0,1);
}

std::string event::createOutput()
{
    return itemPtr->formatOutput();
}

int event::getSenderId()
{
    return itemPtr->getId();
}

std::string event::getSenderMessage()
{
    return itemPtr->getMessage();
}

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

