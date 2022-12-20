#pragma once
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "doodle.h"


unsigned char* flattenImage(cv::Mat& image);

cv::Mat cropAlignTopLeft(cv::Mat& img);

cv::Mat cropResize(cv::Mat& img);

unsigned char* processDoodle(DoodleRaw* doodleRaw);

void createPreprocessedData();
