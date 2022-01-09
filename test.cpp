#include <iostream>
#include <stdint.h>

template <typename T, int N>
char (&Test(T (&array)[N]))[N];

#define arraysize(array) (sizeof(Test(array)))


int main()
{
    int a[10] = {0};
    std::cout << arraysize(a) << "\n";
    return 0;
}