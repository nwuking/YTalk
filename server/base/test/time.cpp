#include "base/Time.h"

#include <iostream>

int main()
{
    YTalk::Time now = YTalk::Time::now();
    std::string localStr = now.timeToStringLocal();
    std::string utcStr = now.timeToStringUTC();

    std::cout << localStr << "\n" << utcStr << "\n";
    return 0;
}