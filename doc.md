# Lemon++/PP whatever

The whole idea with this program is to build out a C++ wrapper for Lemonbar that *could be* easily expanded.


# Design

This design revolves around a multi-producer and single consumer thread system
The items all have their own threads and report to a queue. The bar sits in its own seperate thread
watching the queue. It pops the newest events and outputs them to std::cout.

The program handles the formatting for the output into Lemonbar, including color and orientation

From the main thread, the average program would be shaped like this

'''
int main()
{
    // Predefined items that I found useful already are in place
    // There is also a custom script function that just recieves
    // the output as an std::string
    Time timeItem(location::right);
    Xwindow windowItem(location::center);
    Bspwm desktopItem(location::left);
    CustomScript customItem(location::center, /path/to/script);

    // Purposely placed out of order - it auto organizes
    Bar mainBar(&timeItem, &desktopItem, &windowItem);
}

'''

Items are declared first and their threads are automatically constructed in their constructors. 
Items must have a location declared first.
The bar takes in references to the items and automagically organizes them into the correct order.

# Items

The bar is comprised of "items" that all report to a single queue with data. These items run in seperate threads to allow for
easy updating of individual items. The items all derive from one base Item class. This class includes the id, location and coloring
for the items but also holds a virtual function for the format output (for Lemonbar) that is constructed from the fgColor, bgColor,
placement, and message.

# Events

Items emit events into the queue which contain a message and pointer to the sender item. This Bar thread will call the formatOutput function
from the pointer which returns an std::string formatted for the Lemonbar. This allows for the Bar thread to remain light and avoid a switch
function for all of the items. This also keeps the event type lightweight as it only carries a pointer and a std::string message. 

The event queue is also wrapped around a class to allow for thread safety. I found out the hard way that multiple threads do not work with a queue,
event with only 1 consumer and multiple producers. This queue uses mutexes to allow for only one thread to access it at once

# Bar

The bar thread waits for a condition variable to notified that the queue has been altered, then it locks the queue and reads the event. The formatOutput is called
using the pointer to the item and the output is piped into a waiting output. The overall output is saved after every push to cout to allow for simple updating

# Bar

The bar thread waits for a condition variable to notified that the queue

# Whats the point?

There really isn't one, honestly. I wanted a project to do and this seemed like fun. If there was any goal to this,
it would be to build a singular binary to be *mostly* transferable to other computers. No messing about with dotfiles
or anything stupid like that.


# Extending this program

Honestly good luck. I built this with my use in mind, and I'm a pretty new C++ programmer, so a vast
majority of this is completely wrong or ass-backwards in design.


# Includes

main.cpp > bar.h > items.h > events.h

Problem: 
class Event creates an Item pointer and uses the getId and createFormat functions

class Item uses 


# New way of doing this

Bar stays mostly the same; It waits for the CV to be notified of changes then locks the queue to operate on it.

For now, events are now going to be just a simple pointer and std::string of what needs to be outputted. Items are explained further below

The event queue is now just directly holding the events and nothing else; It keeps the thread safety

Items now have member variables for message, fgColor, bgColor, and location. They also have a member function for formatting the output into message.
This allows for the items to control all of their individual outputting and keeps the design much simpiler. The items will run the formatOutput function,
then push the output into an event along with a pointer to itself. This should simplify the design.
