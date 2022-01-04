// CHIP8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CPU.h"
#include <raylib.h>
#include "CHIP8.h"

constexpr int gScreenWidth = 800;
constexpr int gScreenHeight = 450;

//800 x 450
CPU chip8;
uint8_t screenData[gScreenHeight][gScreenWidth][3];

static uint8_t HandleInput()
{
    if (IsKeyDown(KEY_ONE))return 0x1;
    if (IsKeyDown(KEY_TWO))return 0x2;
    if (IsKeyDown(KEY_THREE))return 0x3;
    if (IsKeyDown(KEY_FOUR))return 0xC;

    if (IsKeyDown(KEY_Q)) return 0x4;
    if (IsKeyDown(KEY_W)) return 0x5;
    if (IsKeyDown(KEY_E)) return 0x6;
    if (IsKeyDown(KEY_R)) return 0xD;

    if (IsKeyDown(KEY_A)) return 0x7;
    if (IsKeyDown(KEY_S)) return 0x8;
    if (IsKeyDown(KEY_D)) return 0x9;
    if (IsKeyDown(KEY_F)) return 0xE;

    if (IsKeyDown(KEY_Z)) return 0xA;
    if (IsKeyDown(KEY_X)) return 0x0;
    if (IsKeyDown(KEY_C)) return 0XB;
    if (IsKeyDown(KEY_V)) return 0xF;

    else return 0x00ff;
}

int main()
{

    //Setup graphics
    InitWindow(gScreenWidth, gScreenHeight, "CHIP8 Emu");
    SetTargetFPS(60);

    //TODO(y): Setup input


    chip8.Initialize();
    chip8.LoadGame("tetris.c8");

    while (!WindowShouldClose())
    {
        {
            auto key = HandleInput();
            chip8.mKey = key;


            chip8.EmulateCycle();

            if (chip8.DrawFlag)
            {
                BeginDrawing();

                ClearBackground(BLACK);
                DrawText("CHIP8 GUI", 100, 100, 16, RAYWHITE);

                //for (int y = 0; y < 32; ++y)
                //    for (int x = 0; x < 64; ++x)
                //        if (chip8.GetGraphics()[(y * 64) + x] == 0)
                //        {
                //            screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;
                //            DrawPixel(x, y, RED);
                //        }
                //        else
                //        {
                //            screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 255;
                //            DrawPixel(x, y, RAYWHITE);
                //        }

                int i, j;
                for (i = 0; i < 64; i++)
                {
                    for (j = 0; j < 32; j++)
                    {
                        if (chip8.GetGraphics()[i][j] == 1)
                        {
                            DrawPixel(i, j, RAYWHITE);
                        }
                    }
                }


                chip8.DrawFlag = false;

                EndDrawing();

            }



        }
    }

    std::cout << "Hello World!\n";
}

