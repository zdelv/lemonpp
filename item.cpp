#include "item.h"

int Item::itemCount = 0;

std::string locToString(location input)
{
    std::string outputLoc; 
    switch(input)
    {
        case(location::left):   outputLoc = "l"; break;
        case(location::right):  outputLoc = "r"; break;
        case(location::center): outputLoc = "c"; break;
        default: std::cout << "Invalid location used!" << std::endl; break;
    }
    return outputLoc;
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);

    if (!pipe) 
        throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) 
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }

    return result;
}

/* void Item::itemPushEvent(event input) */
/* { */
/*     mainEventQueue.push(input); */   
/* } */

location Item::getPlacement()
{
    return this->placement;     
}

