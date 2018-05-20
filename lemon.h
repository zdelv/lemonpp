// Main file for Implementing items and the bar
// This might split into an items.h and bar.h later,
// I kinda want to keep this all in one header, makes 
// it easier to manage.
// By: zdelv

#include "items.h"


// Main class for the Bar, provides the implementation for most interaction needed
class Bar
{
    private:
        // List of pointers to the current items in the bar
        // Is used so far by the constructor only
        std::vector<Item*> itemList;
        std::thread barThread;

        // Privateized implementation for pushing the bar to stdout
        void outputBar();
        // Might be moved out to public so the seperate items can each call this.
        void refreshBar();

        void eventWatchLoop();
        void displayEvents();

    public:
        template<typename... ItemPack>
        Bar(ItemPack... args);
        ~Bar();

        // Will be used as the global creator for the bar
        // Might be called or it could be a hook on the constructor
        void createBar();

        // Would be used to add new items to the bar. Has to be derived from Item
        void addItem(Item& item);

        // Recieves an event from an item. Pushes into bar FIFO
        //void barPushEvent(event input);
};

template<typename... ItemPack>
Bar::Bar(ItemPack... args)
{ 
    (itemList.push_back(args), ...);
    barThread = std::thread(&Bar::eventWatchLoop, this);
    for (int i=0; i < itemList.size(); ++i)
    {
        std::cout << itemList.at(i)->getId() << std::endl;
    }
    std::cout << "Bar constructed" << std::endl;
}

Bar::~Bar()
{
    barThread.join();
}

void Bar::outputBar()
{
    
}

void Bar::refreshBar()
{

}

void Bar::createBar()
{

}

void Bar::addItem(Item& item)
{

}

/* void Bar::barPushEvent(event input) */
/* { */
/*     eventFifo.push(input); */
/* } */

// Continually watches the eventFifo for updates
// The thread will run this loop
// Uses condition_variables and mutexes to keep the CPU low
void Bar::eventWatchLoop()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(m);
        std::cout << "Bar waiting" << std::endl;
        cv.wait(lk);
        std::cout << "Bar locked" << std::endl;

        while (eventFifo.empty() == false)
        {
            displayEvents();
        }

        lk.unlock();
        std::cout << "Bar unlocked" << std::endl;
    }
}

// Displays event from queue and removes it
void Bar::displayEvents()
{
    std::cout << eventFifo.front().message << std::endl; 

    // Remove item from FIFO when done with it
    eventFifo.pop();
}
