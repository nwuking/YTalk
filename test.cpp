#include <iostream>
#include <stdint.h>

class Parent
{
    virtual void foo() {
        std::cout << "Parent";
    }
};

class Child : public Parent
{
    virtual void foo() {
        
    }
};