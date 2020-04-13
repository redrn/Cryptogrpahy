#include <iostream>

namespace util 
{
    template <typename T>
    void print(T vec)
    {
        for (const auto &x : vec)
        {
            std::cout << x;
        }
        std::cout << std::endl;
    }

    template <typename T>
    void printl(T x)
    {
        std::cout << x << std::endl;
    }
}
