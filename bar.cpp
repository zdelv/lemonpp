#include "bar.h"

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
// TODO: Move all of the formatting out to the individual items
//       Its so much cleaner if it isn't built into the bar +
//       there are so many different cases for the formatting
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
                            << "} " << itemList->at(i)->getId() << " %{B-}%{F-}";
        } else 
        {
        *barStream << "%{F" << "#ff0000" << "}%{B" << "#00ff00" \
                        << "} " <<  itemList->at(i)->getId() << " %{B-}%{F-}";
        }
    }
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
    return barEvent.createOutput();    
}


/* void Bar::barPushEvent(event input) */
/* { */
/*     mainEventQueue.push(input); */
/* } */

// Continually watches the mainEventQueue for updates
// The thread will run this loop
// Uses condition_variables and mutexes to keep the CPU low
void Bar::eventWatchLoop()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(m);
        /* std::cout << "Bar waiting" << std::endl; */
        while (mainEventQueue.empty())
        {
            cv.wait(lk);
        }

        // Send two endl's to make the spacing look nice 
        /* std::cout << "Bar locked" << std::endl << std::endl; */

        while (mainEventQueue.empty() == false)
        {
            event newEvent = mainEventQueue.front();
            mainEventQueue.pop();
            /* displayEvents(); */
            //std::cout << formatMessage(mainEventQueue.front()) << std::endl;
            //std::cout << this->currentBar;
        }
        std::cout << std::endl;

        lk.unlock();
        /* std::cout << "Bar unlocked" << std::endl; */
    }
}

// Displays event from queue and removes it
void Bar::displayEvents()
{
    std::cout << "Sender: " << mainEventQueue.front().getSenderId() << std::endl \
        << "Message: " << mainEventQueue.front().getSenderMessage() << std::endl; 

    // Remove item from FIFO when done with it
    mainEventQueue.pop();
}
