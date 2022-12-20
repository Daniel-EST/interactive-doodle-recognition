#include <regex>
#include <iterator>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "doodle.h"
#include "utils.h"
#include "options.h"


std::string getLabel(const std::string file)
{
    std::regex labelRe("(\\w+)\\.bin");
    std::smatch match;

    if (std::regex_search(file.begin(), file.end(), match, labelRe))
        return match[1];

    return std::string{"NULL"};
}

void showDoodles(DoodleRaw* doodleRaw)
{
    std::cout << "key_id: " << doodleRaw->keyId << std::endl;
    std::cout << "country_code: " << doodleRaw->countryCode << std::endl;
    std::cout << "recognized: " << (int) doodleRaw->recognized << std::endl;
    std::cout << "timestamp: " << doodleRaw->timestamp << std::endl;
    std::cout << "n_strokes: " << doodleRaw->nStrokes << std::endl;
    std::cout << "img: [";
    for(auto coord = doodleRaw->imageStrokes.begin(); coord != doodleRaw->imageStrokes.end(); ++coord)
    {
        if(coord != doodleRaw->imageStrokes.begin()) std::cout << ", ";
        std::cout << "(";
        for(auto b = coord->begin(); b != coord->end(); ++b)
        {
            if(b != coord->begin()) std::cout << ", ";
            std::cout << "(";
            for(auto c = b->begin(); c != b->end(); ++c)
            {
                if(c != b->begin()) std::cout << ", ";
                std::cout << *c;
            }
            std::cout << ")";
        } 
    std::cout << ")";
    }
    std::cout << "]" << std::endl;
    std::cout << std::endl;
}

void showDoodles(std::vector<DoodleRaw*> doodlesRaw)
{
    for(DoodleRaw* doodleRaw : doodlesRaw)
        showDoodles(doodleRaw);
}

void showDoodles(unsigned char* doodleProcessed)
{
    std::cout << "data: (";
    for(size_t i{}; i < RESIZE_HEIGHT * RESIZE_WIDTH; i++)
    {
        std::cout << (int) doodleProcessed[i];
        if(i < RESIZE_HEIGHT * RESIZE_WIDTH - 1)
            std::cout << ", ";
    }
    std::cout << ")" << std::endl;
    std::cout << std::endl;
}

void showDoodles(std::vector<unsigned char*> doodlesProcessed)
{
    for(unsigned char* doodleProcessed : doodlesProcessed)
        showDoodles(doodleProcessed);
}

void saveProcessedDoodle(std::ofstream& wf, unsigned char* image)
{
    wf.write((char *) image, RESIZE_WIDTH * RESIZE_HEIGHT);
}