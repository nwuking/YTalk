#include "base/ConfigParse.h"

#include <iostream>

int main()
{
    std::string path = "./config.conf";
    YTalk::ConfigParse p;
    p.parse(path);
    std::cout << "size: " << p.size() << "\n";
    if(p.isExist("date")) {
        std::cout << "date :\n";
    }
    if(p.isExist("email")) {
        std::cout << "email\n";
    }
    if(p.isExist("hahah")) {
        std::cout << "hahha\n";
    }
    std::cout << "size: " << p.size() << "\n";

    int date;
    p.getValue("date", date);
    std::cout << date << "\n";
    return 0;
}