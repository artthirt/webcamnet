#ifndef STRUCTS_H
#define STRUCTS_H

#include <functional>

struct Cntrl{
    int exposure = 0;
    int quality = 20;
    int resolutionid = 0;
};


typedef std::function<void(const Cntrl&)> funsetexposure;

#endif // STRUCTS_H
