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
    Image imageRayLibModelView;
	Texture2D textureModelView;
    cv::Mat img;

    bool showModelView = false;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Doodle Canvas");
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(60);
    
    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    Rectangle CanvasArea
    {
        (float) (GetScreenWidth() / 2) - (IMAGE_WIDTH / 2), 
        (float) (GetScreenHeight() / 2) - (IMAGE_HEIGHT / 2), 
        IMAGE_WIDTH, IMAGE_HEIGHT
    };
    
    BeginTextureMode(target);
        DrawRectangleRec(CanvasArea, BLACK);
    EndTextureMode();


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

        if(CheckCollisionPointRec(GetMousePosition(), CanvasArea))
        {
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                BeginTextureMode(target);
                    DrawCircle(mousePos.x, mousePos.y, LINE_THICKNESS, WHITE);
                EndTextureMode();
            }

            if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                BeginTextureMode(target);
                    DrawCircle(mousePos.x, mousePos.y, LINE_THICKNESS + 4, BLACK);
                EndTextureMode();
            }

            if((IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)))
            {
                image = LoadImageFromTexture(target.texture);
                ImageFlipVertical(&image);
                ImageCrop(&image, CanvasArea);
                ImageColorGrayscale(&image);
                // ImageResize(&image, RESIZE_WIDTH, RESIZE_HEIGHT);

                cv::Mat img(image.height, image.width, CV_8UC1, image.data);
                cv::resize(img, img, cv::Size(RESIZE_WIDTH, RESIZE_HEIGHT), cv::INTER_CUBIC);
        
                img = cropAlignTopLeft(img);
                imgModelView = img;
                
                const auto input = fdeep::tensor_from_bytes((uint8_t*) img.data,
                    static_cast<std::size_t>(RESIZE_HEIGHT),
                    static_cast<std::size_t>(RESIZE_WIDTH),
                    static_cast<std::size_t>(1),
                    0.0f, 255.0f
                );

                // std::cout << "Predict" << std::endl;
                const size_t result = model.predict_class({input});
                // std::cout << resultTranslator[result] << std::endl;
                prediction.clear();
                prediction.append(resultTranslator[result]);
                UnloadImage(image);
            }
        }

        if(IsKeyPressed(KEY_V))
        {
            std::cout << "Showing Model View: " << prediction << std::endl;

            cv::resize(imgModelView, resizedModelView, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT), cv::INTER_CUBIC);
            cv::cvtColor(resizedModelView, imageModelViewCVRGB, cv::COLOR_BGR2RGB);

            imageRayLibModelView.width = imageModelViewCVRGB.cols;
            imageRayLibModelView.height = imageModelViewCVRGB.rows;
            imageRayLibModelView.format = 4;
            imageRayLibModelView.mipmaps = 1;
            imageRayLibModelView.data = (void*) (imageModelViewCVRGB.data);

            textureModelView = LoadTextureFromImage(imageRayLibModelView);

            showModelView = !showModelView;
        }

        if (IsKeyPressed(KEY_C))
        {
            BeginTextureMode(target);
                DrawRectangleRec(CanvasArea, BLACK);
            EndTextureMode();
            prediction.clear();
        }

        if (IsKeyPressed(KEY_F5))
        {
            image = LoadImageFromTexture(target.texture);
            ImageFlipVertical(&image);
            ImageCrop(&image, CanvasArea);
            ExportImage(image, "painting.png");
            UnloadImage(image);
        }

        BeginDrawing();
            ClearBackground(SKYBLUE);
            DrawRectangleRec(CanvasArea, BLACK);
            DrawTextureRec(target.texture, 
                (Rectangle) 
                { 
                    0, 0, 
                    (float)target.texture.width, 
                    (float)-target.texture.height 
                }, 
                (Vector2) { 0, 0 },
                WHITE
            );

            if(CheckCollisionPointRec(GetMousePosition(), CanvasArea))
                DrawCircle(
                    GetMouseX(), GetMouseY(), 
                    LINE_THICKNESS, 
                    ColorAlpha(WHITE, 0.75)
                );
            
            const char* text = TextFormat("%s", prediction.c_str());
            DrawText(text, (GetScreenWidth() / 2) - (MeasureText(text, 32) / 2), 500, 32, RAYWHITE);

            DrawText("Draw your doodle!", (GetScreenWidth() / 2) - (MeasureText("Draw your doodle!", 50) / 2), 100, 50, RAYWHITE);
            
            DrawText("\"V\" - Draw Model View", 10, 620, 20, RAYWHITE);
            DrawText("\"F5\" - Screenshot", 10, 650, 20, RAYWHITE);
            DrawText("\"C\" - Clear canvas", 10, 680, 20, RAYWHITE);
            
            if(showModelView)
            {
                DrawTexture(
                    textureModelView, 
                    (SCREEN_WIDTH / 2) - (IMAGE_WIDTH / 2), (SCREEN_HEIGHT / 2) - (IMAGE_HEIGHT / 2), 
                    RAYWHITE
                );
            }

            DrawRectangleLinesEx(
                (Rectangle)
                {
                    CanvasArea.x - LINE_THICKNESS,
                    CanvasArea.y - LINE_THICKNESS,
                    CanvasArea.width + LINE_THICKNESS * 3,
                    CanvasArea.height + LINE_THICKNESS * 3,
                }, 
                LINE_THICKNESS,
                SKYBLUE
            );
        EndDrawing();
    }

    UnloadRenderTexture(target);
    UnloadTexture(texture);
    UnloadTexture(textureModelView);
    CloseWindow();
}
