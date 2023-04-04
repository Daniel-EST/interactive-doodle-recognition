#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <map>
#include <random>
#include <algorithm>

#include <fdeep/fdeep.hpp>
#include <raylib.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "options.h"
#include "preprocess.h"

void startCanvas(const fdeep::model& model)
{
    std::string prediction{""};

    Image image;
	Texture2D texture;


    cv::Mat imgModelView;
    cv::Mat resizedModelView;
    cv::Mat imageModelViewCVRGB;
	Texture2D textureModelView;
    cv::Mat img;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Doodle Canvas");
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(60);
    
    RenderTexture2D target = LoadRenderTexture(IMAGE_WIDTH, IMAGE_HEIGHT);

    BeginTextureMode(target);
        DrawRectangleRec(
            { 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT }, 
            BLACK
        );
    EndTextureMode();

    Rectangle canvasArea = { (GetScreenWidth() - IMAGE_WIDTH) / 2.0f, (GetScreenHeight() - IMAGE_HEIGHT) / 2.0f, IMAGE_WIDTH, IMAGE_HEIGHT };

    std::map<size_t, std::string> resultTranslator;
    resultTranslator[0] = "Angel";
    resultTranslator[1] = "Bicycle";
    resultTranslator[2] = "Cat";
    resultTranslator[3] = "Alarm Clock";
    resultTranslator[4] = "Cup";
    resultTranslator[5] = "Diamond";
    resultTranslator[6] = "Dolphin";
    resultTranslator[7] = "Door";
    resultTranslator[8] = "Eye";
    resultTranslator[9] = "Eyeglasses";
    resultTranslator[10] = "Hat";
    resultTranslator[11] = "Headphones";
    resultTranslator[12] = "Helicopter";
    resultTranslator[13] = "House";
    resultTranslator[14] = "Octopus";
    resultTranslator[15] = "House Plant";
    resultTranslator[16] = "Popsicle";
    resultTranslator[17] = "Ship";
    resultTranslator[18] = "Umbrella";
    resultTranslator[19] = "Windmill";

    while(!WindowShouldClose())
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 mousePosRelative = { mousePos.x - canvasArea.x, mousePos.y - canvasArea.y };

        if(CheckCollisionPointRec(mousePos, canvasArea))
        {
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                BeginTextureMode(target);
                    DrawCircle(
                        mousePosRelative.x,
                        mousePosRelative.y,
                        LINE_THICKNESS, WHITE
                    );
                EndTextureMode();
            }

            if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                BeginTextureMode(target);
                    DrawCircle(
                        mousePosRelative.x,
                        mousePosRelative.y,
                        LINE_THICKNESS + 4, BLACK
                    );
                EndTextureMode();
            }

            if((IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)))
            {
                image = LoadImageFromTexture(target.texture);
                ImageFlipVertical(&image);
                ImageCrop(&image, canvasArea);
                ImageColorGrayscale(&image);

                cv::Mat img(image.height, image.width, CV_8UC1, image.data);
                cv::resize(img, img, cv::Size(RESIZE_WIDTH, RESIZE_HEIGHT), cv::INTER_CUBIC);
        
                imgModelView = img;
                
                const auto input = fdeep::tensor_from_bytes((uint8_t*) img.data,
                    static_cast<std::size_t>(RESIZE_HEIGHT),
                    static_cast<std::size_t>(RESIZE_WIDTH),
                    static_cast<std::size_t>(1),
                    0.0f, 255.0f
                );

                const size_t result = model.predict_class({ input });

                prediction.clear();
                prediction.append(resultTranslator[result]);
                UnloadImage(image);
            }
        }

        if (IsKeyPressed(KEY_C))
        {
            BeginTextureMode(target);
                DrawRectangleRec({ 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT }, BLACK);
            EndTextureMode();
            prediction.clear();
        }

        if (IsKeyPressed(KEY_F5))
        {
            image = LoadImageFromTexture(target.texture);
            ImageFlipVertical(&image);
            ImageCrop(&image, canvasArea);
            ExportImage(image, "painting.png");
            UnloadImage(image);
        }

        BeginDrawing();
            ClearBackground(SKYBLUE);
    
            DrawTextureRec(target.texture,
                { 0, 0, canvasArea.width, -canvasArea.height },
                { canvasArea.x , canvasArea.y },
                WHITE
            );

            if(CheckCollisionPointRec(mousePos, canvasArea))
            {
                DrawCircle(
                    mousePos.x, mousePos.y, 
                    LINE_THICKNESS, 
                    ColorAlpha(WHITE, 0.75)
                );
            }
        
            const char* predictionText = TextFormat("%s", prediction.c_str());
            DrawText(predictionText, (GetScreenWidth() / 2) - (MeasureText(predictionText, 32) / 2), 500, 32, RAYWHITE);

            const char* titleText = "Draw your doodle!";
            DrawText(titleText, (GetScreenWidth() / 2) - (MeasureText(titleText, 50) / 2), 100, 50, RAYWHITE);
            
            DrawText("\"F5\" - Screenshot", 10, 650, 20, RAYWHITE);
            DrawText("\"C\" - Clear canvas", 10, 680, 20, RAYWHITE);

        EndDrawing();
    }

    UnloadRenderTexture(target);
    UnloadTexture(texture);
    UnloadTexture(textureModelView);
    CloseWindow();
}
