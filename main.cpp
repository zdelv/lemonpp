#include "lemon.h"

int main() 
{
    // Future use of api
    Time x(location::center);
    Time r(location::center);
    Time l(location::center);
    Time y(location::right);
    Time z(location::left);

    // Should order to g2, x1, z4, y3, r5, m6
    Bar b(&x, &y, &z, &r, &l);

    /* while (true) */
    /* { */
    /*     std::cout << "Event FIFO has " << eventFifo.size() << " events" << std::endl; */
    /*     std::this_thread::sleep_for(2s); */
    /* } */

    std::this_thread::sleep_for(10s);

    //xwindow y(center);
    //bspwm z(left);

    //Bar mainbar(x, y, z);
}
