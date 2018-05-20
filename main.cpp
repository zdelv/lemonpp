#include "lemon.h"

int main() 
{
    // Future use of api
    Time x(location::left);

    Bar b(&x);

    while (true)
    {
        std::cout << "Event FIFO has " << eventFifo.size() << " events" << std::endl;
        std::this_thread::sleep_for(2s);
    }

    std::this_thread::sleep_for(10s);

    //xwindow y(center);
    //bspwm z(left);

    //Bar mainbar(x, y, z);
}
