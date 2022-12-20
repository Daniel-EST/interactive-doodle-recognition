#pragma once
#include <string>
#include <vector>

#include "doodle.h"
 

std::string getLabel(const std::string file);

void showDoodles(DoodleRaw* doodleRaw);

void showDoodles(std::vector<DoodleRaw*> doodlesRaw);

void showDoodles(unsigned char* DoodleProcessed);

void showDoodles(std::vector<unsigned char*> doodlesProcessed);

void saveProcessedDoodle(std::ofstream& wf, unsigned char* image);
