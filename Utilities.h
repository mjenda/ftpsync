#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>

class QString;

namespace Utilities
{

void loadbar(unsigned int x, unsigned int n, unsigned int w = 100);

bool dirIsNotRefdir(const QString& dir);

template <typename T> void dealocate(std::vector<T>& vec)
{
    std::vector<T>().swap(vec);
}

}

#endif // UTILITIES_H
