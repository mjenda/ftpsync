#include "Utilities.h"
#include <iostream>
#include <iomanip>
#include <QString>

namespace Utilities
{

void loadbar(unsigned int x, unsigned int n, unsigned int w)
{
    float ratio  =  x/(float)n;
    int   c      =  ratio * w;

    std::cout << std::setw(3) << (int)(ratio*100) << "% [";
    for (int x=0; x<c; x++) std::cout << "=";
    for (int x=c; x<w; x++) std::cout << " ";
    std::cout << "]\r" << std::flush;

    if (x == n)
    {
        std::cout << std::endl;
    }
}

bool dirIsNotRefdir(const QString &dir)
{
    return dir != "." && dir != "..";
}

} // namespace Utilities

