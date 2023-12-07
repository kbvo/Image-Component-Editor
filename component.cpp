#include "component.h"
#include <iostream>
#include <vector>

using namespace std;

// Complete - Do not alter
Component::Component()
{
    // ulOrig and ulNew use Location's default constructor to
    //  already initialize to -1,-1

    label = -1;
    height = 0;
    width = 0;
}

// Complete - Do not alter
Component::Component(Location origUL, int h, int w, int mylabel)
{
    // Initialize data members (set ulOrig and ulNew to origUL input)
    ulOrig = origUL;
    ulNew = origUL;
    label = mylabel;
    height = h;
    width = w;
}

// Complete - Do not alter
Component::~Component()
{
}


