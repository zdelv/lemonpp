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
        // evenOrg is used to organize by left, center then right then
        // by id number (low->high)
        std::shared_ptr<std::vector<Item*>> itemList;

        // Represents what was last sent to the bar. Continually updated
        std::string currentBar;

        // Generic thread for the bar. Allows for the main thread to stay active
        std::thread barThread;

        // Privateized implementation for pushing the bar to stdout
        void outputBar();
        // Might be moved out to public so the seperate items can each call this.
        void refreshBar();

        void eventWatchLoop();
        void displayEvents();

        // Main event organizer. This takes the current eventFifo and
        // itemList and writes the next bar to currentBar. It ensures
        // that the bar contains all of the items.
        void itemOrg();
        
        // Generic message formatter. This might be moved out to the
        // individual items, as it would make specific formatting easier
        std::string formatMessage(event barEvent);

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
    : itemList(std::make_shared<std::vector<Item*>>())
{ 
    (itemList->push_back(args), ...);
    itemOrg();
    for (int i=0; i < itemList->size(); ++i)
    {
        std::cout << itemList->at(i)->getId() << std::endl;
    }
    createBar();
    std::cout << "Bar constructed" << std::endl;
    //Start the thread last to make sure everything is initialized first
    barThread = std::thread(&Bar::eventWatchLoop, this);
}

Bar::~Bar()
{
    barThread.join();
}

// Pushes the currentBar to cout
void Bar::outputBar()
{
     
}

void Bar::refreshBar()
{

}

// Calls itemOrg to order the items then builds the new bar
// This includes all new and old events
// Places this new bar into currentBar
// ONLY call this at initialization, use refreshBar everywhere else
void Bar::createBar()
{
    std::unique_ptr<std::stringstream> barStream = std::make_unique<std::stringstream>();

    std::string currentSide = "none";
    for (int i=0; i < itemList->size(); ++i)
    {
        if (currentSide != locToString(itemList->at(i)->getPlacement()))
        {
            currentSide = locToString(itemList->at(i)->getPlacement());
            *barStream << "%{" << locToString(itemList->at(i)->getPlacement()) << "}%{F" << "#ff0000" << "}%{B" << "#00ff00" \
                            << "} " << "test" << i << " %{B-}%{F-}";
        } else 
        {
        *barStream << "%{F" << "#ff0000" << "}%{B" << "#00ff00" \
                        << "} " << "test" << i << " %{B-}%{F-}";
        }
    }
    currentSide = "none";
    this->currentBar = barStream->str();
}

// Organize the itemList to be used by createBar
// Orders by orientation, then id:
// Left, Center and Right, then inside of those its smallest to largest
// TODO: Make a single iterator, should be easy
// TODO: I think this could be one large for loop or even a single sort
void Bar::itemOrg()
{
    // This should die after the function exits
    std::unique_ptr<std::vector<Item*>> newItemList = std::make_unique<std::vector<Item*>>();           
    int leftLength = 0, centerLength = 0, rightLength = 0;
    //auto newIt = newItemList.begin();

    for (int i=0; i < itemList->size(); ++i)
    {
        if (itemList->at(i)->getPlacement() == location::left)
        {
            newItemList->push_back(itemList->at(i));
            leftLength += 1;
        }
        std::sort(newItemList->begin(), newItemList->begin() + leftLength, 
                [](Item* first, Item* second) { return first->getId() < second->getId(); });
    }
    for (int i=0; i < itemList->size(); ++i)
    {
        if (itemList->at(i)->getPlacement() == location::center)
        {
            newItemList->push_back(itemList->at(i));
            centerLength += 1;
        }
        std::sort(newItemList->begin() + leftLength, newItemList->begin() + leftLength + centerLength, 
                [](Item* first, Item* second) { return first->getId() < second->getId(); });
    }
    for (int i=0; i < itemList->size(); ++i)
    {
        if (itemList->at(i)->getPlacement() == location::right)
        {
            newItemList->push_back(itemList->at(i));
            rightLength += 1;
        }
        std::sort(newItemList->begin() + leftLength + centerLength, newItemList->begin() + leftLength + centerLength + rightLength, 
                [](Item* first, Item* second) { return first->getId() < second->getId(); });
    }
    *itemList = std::move(*newItemList);
}

void Bar::addItem(Item& item)
{
    
} 

std::string Bar::formatMessage(event barEvent)
{
    std::string formattedMessage;
    formattedMessage = "%{" + barEvent.placement + "}%{F" + barEvent.fgColor + "}%{B" + barEvent.bgColor \
                        + "} " + barEvent.message + " %{F-}%{B-}";
    
    /* std::cout << "%{F" << barEvent.fgColor << "}%{B" << barEvent.bgColor << \ */
    /*     "} " << barEvent.message << " %{F-}%{B-}" << std::endl; */

    return formattedMessage;    
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
        /* std::cout << "Bar waiting" << std::endl; */
        cv.wait(lk);
        // Send two endl's to make the spacing look nice 
        /* std::cout << "Bar locked" << std::endl << std::endl; */
        itemOrg();

        while (eventFifo.empty() == false)
        {
            /* displayEvents(); */
            //std::cout << formatMessage(eventFifo.front()) << std::endl;
            std::cout << this->currentBar;
            eventFifo.pop();
        }
        std::cout << std::endl;

        lk.unlock();
        /* std::cout << "Bar unlocked" << std::endl; */
    }
}

// Displays event from queue and removes it
void Bar::displayEvents()
{
    std::cout << "Sender: " << eventFifo.front().senderId << std::endl \
        << "Message: " << eventFifo.front().message << std::endl; 

    // Remove item from FIFO when done with it
    eventFifo.pop();
}
