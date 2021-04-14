#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <functional>

struct Cntrl{
    int exposure = 0;
    int quality = 20;
    int resolutionid = 0;
};


typedef std::function<void(const Cntrl&)> funsetexposure;
typedef std::function<void(const std::string&, uint16_t)> funsetaddress;

#endif // STRUCTS_H
