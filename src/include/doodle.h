#pragma once
#include <vector>
#include <string>

#include "options.h"

#pragma pack(push, r1, 1)
struct DoodleRaw
{
    unsigned long long keyId;
    char countryCode[3];
    signed char recognized;
    unsigned int timestamp;
    unsigned short nStrokes;
    std::vector<std::vector<std::vector<int>>> imageStrokes;
};
#pragma pack(pop, r1)

#pragma pack(push, r1, 1)
struct DoodleProcessed
{
    unsigned char data[RESIZE_WIDTH * RESIZE_HEIGHT];
};
#pragma pack(pop, r1)
