#include "MathLib.hpp"

template <class T, class U>
T pow(T x, U n)
{
    T ret = 1;
    while (n > 0)
    {
        if (n & 1)
        {
            ret *= x;
        }
        n >>= 1;
        x *= x;
    }
    return ret;
}

template int pow<int, int>(int, int);
template int pow<int, long long>(int, long long);
template long long pow<long long, int>(long long, int);
template long long pow<long long, unsigned int>(long long, unsigned int);
template long long pow<long long, long long>(long long, long long);