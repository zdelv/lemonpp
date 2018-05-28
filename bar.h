#pragma once
#ifndef BAR_H
#define BAR_H

// Main file for Implementing items and the bar
// This might split into an items.h and bar.h later,
// I kinda want to keep this all in one header, makes 
// it easier to manage.
// By: zdelv

#include "userItems.h"


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

        // This currently only organizes the itemList to be in order of orientation then id
        void itemOrg();
        
        // Generic message formatter. This might be moved out to the
        // individual items, as it would make specific formatting easier
        std::string formatMessage(event barEvent);

    public:
        template<typename... ItemPack>
        Bar(ItemPack... args)
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
        ~Bar();

        // Will be used as the global creator for the bar
        // Might be called or it could be a hook on the constructor
        void createBar();

        // Would be used to add new items to the bar. Has to be derived from Item
        void addItem(Item& item);

        // Recieves an event from an item. Pushes into bar FIFO
        //void barPushEvent(event input);
};
#endif
