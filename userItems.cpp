#include "userItems.h"

Time::Time(location _placement)
    : Item(_placement)
{
    timeNow = std::time(0);
    dateNow = std::localtime(&timeNow);
    eventThread = std::thread(&Time::timeEventLoop, this);
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
    std::stringstream timeStream;

    timeStream << "%{F" << intHextoString(0xff00ff) << "}%{B" << "#00ff00" \
                    << "} " << this->msg << " %{B-}%{F-}";

    lastOutput = timeStream.str();
    return timeStream.str();
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

        event time(this, "test");

        mainEventQueue.push(time);
        /* std::cout << "Pushed" << std::endl; */

        cv.notify_one();
        /* std::cout << "Time thread notifies" << std::endl; */

        std::this_thread::sleep_for(2s);
    }
}
