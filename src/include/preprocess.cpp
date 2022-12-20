#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>

#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include <sys/stat.h>
#include <sys/types.h>

#include "doodle.h"
#include "read.h"
#include "preprocess.h"
#include "options.h"
#include "utils.h"


unsigned char* flattenImage(cv::Mat& image)
{
    unsigned char* flattenImg = new unsigned char[RESIZE_WIDTH * RESIZE_HEIGHT];
    
    for(int y{}; y < RESIZE_HEIGHT; y++)
    {
        for(int x{}; x < RESIZE_WIDTH; x++)
            flattenImg[(y * RESIZE_HEIGHT) + x] = image.at<unsigned char>(y,x);
    }

    return flattenImg;
}

cv::Mat cropAlignTopLeft(cv::Mat& img)
{
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    
    std::vector<std::vector<cv::Point>> contours_poly(contours.size());
    
    std::vector<cv::Rect> boundRect(contours.size());

    int minX = img.rows - 1;
    int maxX{};
    int minY = img.cols - 1;
    int maxY{};

    for(size_t i = 0; i < contours.size(); i++)
    {
        cv::approxPolyDP(contours[i], contours_poly[i], 3, true);
        boundRect[i] = cv::boundingRect(contours_poly[i]);
        
        if(boundRect[i].br().x > maxX)
            maxX = boundRect[i].br().x;

        if(boundRect[i].br().y > maxY)
            maxY = boundRect[i].br().y;
        
        if(boundRect[i].tl().x <  minX)
            minX = boundRect[i].tl().x;
        
        if(boundRect[i].tl().y <  minY)
            minY = boundRect[i].tl().y;
    }

    // cv::imshow("No Contours", img);
    // cv::waitKey(0);

    // cv::rectangle(img, cv::Point_<int>(minX, minY), cv::Point_<int>(maxX, maxY), cv::Scalar(255, 0, 0), 2);
    // cv::imshow("Contours", img);
    // cv::waitKey(0);


    cv::Mat out = cv::Mat::zeros(img.size(), img.type());
    cv::Mat cropped = img(cv::Range(minY, maxY), cv::Range(minX, maxX));
    cropped.copyTo(out(cv::Range(0, maxY - minY), cv::Range(0, maxX - minX)));
   
    // cv::imshow("Crop", out);
    // cv::waitKey(0);

    return out;
}

cv::Mat cropResize(cv::Mat& img)
{
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    
    std::vector<std::vector<cv::Point>> contours_poly(contours.size());
    
    std::vector<cv::Rect> boundRect(contours.size());

    int minX = img.rows - 1;
    int maxX{};
    int minY = img.cols - 1;
    int maxY{};

    for(size_t i = 0; i < contours.size(); i++)
    {
        cv::approxPolyDP(contours[i], contours_poly[i], 3, true);
        boundRect[i] = cv::boundingRect(contours_poly[i]);
        
        if(boundRect[i].br().x > maxX)
            maxX = boundRect[i].br().x;

        if(boundRect[i].br().y > maxY)
            maxY = boundRect[i].br().y;
        
        if(boundRect[i].tl().x <  minX)
            minX = boundRect[i].tl().x;
        
        if(boundRect[i].tl().y <  minY)
            minY = boundRect[i].tl().y;
    }

    // cv::imshow("No Contours", img);
    // cv::waitKey(0);

    // cv::rectangle(img, cv::Point_<int>(minX, minY), cv::Point_<int>(maxX, maxY), cv::Scalar(255, 0, 0), 2);
    // cv::imshow("Contours", img);
    // cv::waitKey(0);

    cv::Mat cropped = img(cv::Range(minY, maxY), cv::Range(minX, maxX));
    // cv::imshow("Crop", cropped);
    // cv::waitKey(0);

    return cropped;
}

unsigned char* processDoodle(DoodleRaw* doodleRaw)
{
    cv::Point pStart;
    cv::Point pEnd;

    cv::Mat image(
        IMAGE_WIDTH, IMAGE_HEIGHT, 
        CV_8UC1,
        cv::Scalar(0)
    );

    for(int stroke = 0; stroke < doodleRaw->nStrokes; stroke++)
    {

        pEnd.x = doodleRaw->imageStrokes[stroke][0][0];
        pEnd.y = doodleRaw->imageStrokes[stroke][1][0];

        for(int point = 0; point < doodleRaw->imageStrokes[stroke][0].size(); point++)
        {
            pStart.x = doodleRaw->imageStrokes[stroke][0][point];
            pStart.y = doodleRaw->imageStrokes[stroke][1][point];

            cv::line(
                image,
                pStart, pEnd, 
                cv::Scalar(255), 
                LINE_THICKNESS,
                cv::LINE_8
            );

            pEnd.x = pStart.x;
            pEnd.y = pStart.y;
        }
    }
    
    cv::resize(image, image, cv::Size(RESIZE_WIDTH, RESIZE_HEIGHT), cv::INTER_CUBIC);

    unsigned char* flattened = flattenImage(image);
    
    return flattened;
}

void createPreprocessedData()
{
    DoodleRaw* doodleRaw = nullptr;
    unsigned char* doodleProcessed = nullptr;
    
    std::string savePath = "./data/processed/";

    std::default_random_engine generator;
    std::uniform_real_distribution<double> rng(0.0, 1.0);
    double foldProb;
    double trainProb;

    std::cout << "\nReading Data Files." << std::endl;
    for(const auto& entry : std::filesystem::directory_iterator("./data/raw/"))
    {
        if(!entry.path().string().compare("./data/raw/.DS_Store"))
            continue;

        std::cout << "\nReading File: " << entry.path().c_str() << std::endl;
        std::ifstream rf(entry.path().c_str(), std::ios::binary);
        std::string label = getLabel(entry.path().string());

        mkdir((savePath + label + "/").c_str(), 0777);
        std::ofstream wfTest(savePath + label + "/" + "test.bin", std::ios::binary);
        std::ofstream wfValidation(savePath + label + "/" + "validation.bin", std::ios::binary);

        mkdir((savePath + label + "/" + "train/").c_str(), 0777);
        std::ofstream wfFold01(savePath + label + "/" + "train/01.bin", std::ios::binary);
        std::ofstream wfFold02(savePath + label + "/" + "train/02.bin", std::ios::binary);
        std::ofstream wfFold03(savePath + label + "/" + "train/03.bin", std::ios::binary);
        std::ofstream wfFold04(savePath + label + "/" + "train/04.bin", std::ios::binary);
        std::ofstream wfFold05(savePath + label + "/" + "train/05.bin", std::ios::binary);

        while(true)
        {
            if(!rf) break;

            // std::cout << "Reading Doodle: ";
            doodleRaw = readDoodle(rf);
            if(doodleRaw->nStrokes==0) break;
            // std::cout << "Read;" << std::endl;
            // showDoodles(doodleRaw);


            // std::cout << "Processing Doodle: ";
            doodleProcessed = processDoodle(doodleRaw);
            // std::cout << "Processed;" << std::endl;
            // showDoodles(doodleProcessed);

            // std::cout << "Saving Doodle: ";
            trainProb = rng(generator);
            if(trainProb < 0.1)
            {
                saveProcessedDoodle(wfTest, doodleProcessed);
                // std::cout << "Saved (Test);" << std::endl;
            } 
            else if (trainProb < 0.2)
            {
                saveProcessedDoodle(wfValidation, doodleProcessed);
                // std::cout << "Saved (Validation);" << std::endl;    
            }
            else
            {
                foldProb = rng(generator);
                if(foldProb < 0.2)
                {
                    saveProcessedDoodle(wfFold01, doodleProcessed);
                    // std::cout << "Saved (Fold01);" << std::endl;
                }
                else if (foldProb < 0.4)
                {
                    saveProcessedDoodle(wfFold02, doodleProcessed);
                    // std::cout << "Saved (Fold02);" << std::endl;
                }
                else if (foldProb < 0.6)
                {
                    saveProcessedDoodle(wfFold03, doodleProcessed);
                    // std::cout << "Saved (Fold03);" << std::endl;
                }
                else if (foldProb < 0.8)
                {
                    saveProcessedDoodle(wfFold04, doodleProcessed);
                    // std::cout << "Saved (Fold04);" << std::endl;
                }
                else
                {
                    saveProcessedDoodle(wfFold05, doodleProcessed);
                    // std::cout << "Saved (Fold05);" << std::endl;
                }
            }
            
            delete doodleRaw;
            delete doodleProcessed;
        }

        std::cout << "Done." << std::endl;
        rf.close();
        wfTest.close();
        wfValidation.close();
        wfFold01.close();
        wfFold02.close();
        wfFold03.close();
        wfFold04.close();
        wfFold05.close();
    }
}
