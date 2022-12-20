#include <fstream>
#include <vector>
#include <random>

#include "doodle.h"
#include "read.h"
#include "utils.h"


DoodleRaw* readDoodle(std::ifstream& rf)
{
    DoodleRaw* newDoodle = new DoodleRaw;

    rf.read((char*) &newDoodle->keyId, sizeof(newDoodle->keyId));

    rf.read((char*) &newDoodle->countryCode, sizeof(char[2]));
    newDoodle->countryCode[2] = '\0';

    rf.read((char*) &newDoodle->recognized, sizeof(newDoodle->recognized));

    rf.read((char*) &newDoodle->timestamp, sizeof(newDoodle->timestamp));

    rf.read((char*) &newDoodle->nStrokes, sizeof(newDoodle->nStrokes));

    for (int stroke{}; stroke < newDoodle->nStrokes; stroke++)
    {
        unsigned short nPoints;
        rf.read((char*) &nPoints, sizeof(nPoints));

        unsigned char x[nPoints];
        rf.read((char*) &x, nPoints);

        unsigned char y[nPoints];
        rf.read((char*) &y, nPoints);

        std::vector<int> xVect;
        std::vector<int> yVect;

        for (int point{}; point < nPoints; point++)
        {
            xVect.push_back((int) x[point]);
            yVect.push_back((int) y[point]);
        }

        newDoodle->imageStrokes.push_back(
            std::vector<std::vector<int>> { xVect, yVect }
        );
    }

    return newDoodle;
}