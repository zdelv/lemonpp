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


# Whats the point?

There really isn't one, honestly. I wanted a project to do and this seemed like fun. If there was any goal to this,
it would be to build a singular binary to be *mostly* transferable to other computers. No messing about with dotfiles
or anything stupid like that.


# Extending this program

Honestly good luck. I built this with my use in mind, and I'm a pretty new C++ programmer, so a vast
majority of this is completely wrong.
