#include "CPU.h"
#include <fstream>
#include <iostream>

constexpr unsigned int FontSetSize = 80;
constexpr unsigned int Flag = 15;

static constexpr unsigned char fontset[FontSetSize] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void CPU::Initialize()
{

    mProgramCounter = 0x200;
    mOpcode = 0;
    mI = 0;
    mStackPointer = 0;

    // Clear display
    // Clear stack
    // Clear registers V0-VF
    // Clear memory

    // uh not from 0 probably
    for (uint8_t i = 0; i < 80; ++i)
    {
        mMemory[i] = fontset[i];
    }
}

void CPU::EmulateCycle()
{

    // Fetch Opcode

    mOpcode = mMemory[mProgramCounter] << 8 | mMemory[mProgramCounter + 1];
    // TODO: Decode Opcode
    const auto X = (mOpcode & 0x0F00) >> 8;
    const auto Y = (mOpcode & 0x00F0) >> 4;

    switch (mOpcode & 0xF000)
    {

        case 0x0000:
            switch (mOpcode & 0x000F)
            {

                case 0x0000: // Clear screen
                    for (auto &arr : mGraphics)
                    {
                        arr.fill(0);
                    }
                    DrawFlag = true;
                    mProgramCounter += 2;
                    break;

                case 0x000E: // Return from subroutine
                    --mStackPointer;
                    mProgramCounter = mStack[mStackPointer];
                    mProgramCounter += 2;
                    break;


            }
            break;

        case 0x1000: // JMP
            mProgramCounter = mOpcode & 0x0FFF;
            break;


        case 0x2000:
            mStack[mStackPointer] = mProgramCounter;
            ++mStackPointer;
            mProgramCounter = mOpcode & 0x0FFF;
            break;

        case 0x3000:
            if (mV[X] == (mOpcode & 0x00FF)) mProgramCounter += 4;
            else                 mProgramCounter += 2;

            break;

        case 0x4000:

            if (mV[X] != (mOpcode & 0x00FF)) mProgramCounter += 4;
            else                 mProgramCounter += 2;

            break;

        case 0x5000:
            if (mV[Y] == mV[X]) mProgramCounter += 4;
            else                 mProgramCounter += 2;

            break;

        case 0x6000:
            mV[X] = mOpcode & 0x00FF;
            mProgramCounter += 2;
            break;

        case 0x7000:
            mV[X] += mOpcode & 0x00FF;
            mProgramCounter += 2;
            break;

        case 0x8000: // ALU
        {

            auto const aluop = mOpcode & 0x000F;
            switch (aluop)
            {

                case 0x0000: //  X = Y
                    mV[X] = mV[Y];
                    mProgramCounter += 2;

                    break;

                case 0x0001: // X = X | Y
                    mV[X] |= mV[Y];
                    mProgramCounter += 2;

                    break;

                case 0x0002: // X = X & Y
                    mV[X] &= mV[Y];
                    mProgramCounter += 2;

                    break;

                case 0x003: // X = X xor Y
                    mV[X] ^= mV[Y];
                    mProgramCounter += 2;

                    break;

                case 0x004: // X = X + Y
                    if (mV[X] + mV[Y] > 255) //overflow
                    {
                        mV[Flag] = 1;

                    }
                    else
                    {
                        mV[Flag] = 0;
                    }

                    mV[X] += mV[Y];
                    mProgramCounter += 2;

                    break;

                case 0x005: // X = X - Y
                    if (mV[X] < mV[Y]) //underflow
                    {
                        mV[Flag] = 0;
                    }
                    else
                    {
                        mV[Flag] = 1;

                    }

                    mV[X] -= mV[Y];
                    mProgramCounter += 2;

                    break;

                case 0x006: // X = X // 2

                    if (mV[X] % 2 == 0)
                    {
                        mV[Flag] = 0;
                    }
                    else
                    {
                        mV[Flag] = 1;
                    }

                    mV[X] = mV[X] >> 1;
                    mProgramCounter += 2;

                    break;

                case 0x007:

                    if (mV[Y] > mV[X])
                    {
                        mV[Flag] = 0;
                    }
                    else
                    {
                        mV[Flag] = 1;

                    }


                    mV[X] -= mV[Y];
                    mProgramCounter += 2;

                    break;

                case 0x00E:

					mV[Flag] = mV[(mOpcode & 0x0F00) >> 8] >> 7;
					mV[(mOpcode & 0x0F00) >> 8] <<= 1;
					
                    mProgramCounter += 2;

                    break;

                default:
                    std::cerr << "unable to parse ALU opcode " << std::hex << aluop << '\n';
                    break;
            }

        }
        break;

        case 0x9000:
            if (mV[X] != mV[Y]) mProgramCounter += 4;
            else
            {
                mProgramCounter += 2;
            }

            break;

        case 0xA000:
            mI = mOpcode & 0x0FFF;
            mProgramCounter += 2;
            break;

        case 0xB000:
            mProgramCounter = mV[0] + (mOpcode & 0x0FFF);
            break;

        case 0xC000:
            mV[X] = (rand() % 255) & (mOpcode & 0x00FF);
            mProgramCounter += 2;
            break;

        case 0xD000:
        {
            int height = mOpcode & 0x000F;
            int x_coord = mV[X];
            int y_coord = mV[Y];

            // because the sprite is represented by hexadecimal numbers
            // bitwise operators are necessary to obtain each pixel
            int ands[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

            // set carry flag to 0
            mV[Flag] = 0;
            // drawing loop
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < 64; j++)
                {
                    // allows sprite to wrap around screen
                    if (x_coord + j == 64)
                    {
                        x_coord = -j;
                    }
                    if (y_coord + i == 32)
                    {
                        y_coord = -i;
                    }

                    // set carry flag to 1 if a sprite changes from set to unset
                    if (mGraphics[x_coord + j][y_coord + i] == 1 &&
                        ((mMemory[mI + i] & ands[j]) >> (8 - j - 1)) == 1)
                    {
                        mV[Flag] = 1;
                    }

                    // bitwise operations decode each bit of sprite and XOR with the current pixel on screen
                    mGraphics[x_coord + j][y_coord + i] = mGraphics[x_coord + j][y_coord + i] ^
                        ((mMemory[mI + i] & ands[j]) >> (8 - j - 1));
                }
                x_coord = mV[X];
                y_coord = mV[Y];
            }

            DrawFlag = true;
            mProgramCounter += 2;
        }
        break;

        case 0xE000:
            switch (mOpcode & 0x00FF)
            {
                case 0x009E:
                    if (mKey == mV[X]) mProgramCounter += 4;
                    else mProgramCounter += 2;
                    break;
                case 0x00A1:
                    if (mKey != mV[X]) mProgramCounter += 2;
                    else mProgramCounter += 2;
                    break;
                default:

                std::cerr << "Unknown opcode" << std::hex << mOpcode << '\n';
            }
            break;
			
        case 0xF000:
        {
            switch (mOpcode & 0x00FF)
            {


                case 0x0007:
                    mV[X] = mDelayTimer;
                    mProgramCounter += 2;
                    break;

                case 0x000A:

                    if (!mKey)
                    {
                        return;
                    }
                    else
                    {
                        mV[X] = mKey;
                        mProgramCounter += 2;
                    }
                    break;
                case 0x0015:

                    mDelayTimer = mV[X];
                    mProgramCounter += 2;
                    break;
                case 0x0018:

                    mSoundTimer = mV[X];
                    mProgramCounter += 2;
                    break;
                case 0x001E:
                    if (mI + mV[(mOpcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
                        mV[0xF] = 1;
                    else
                        mV[0xF] = 0;
                    mI += mV[(mOpcode & 0x0F00) >> 8];
                    mProgramCounter += 2;
                    break;

                case 0x0029:
                    mI = mV[X] * 5;
                    mProgramCounter += 2;
                    break;

                case 0x0033:
                    mMemory[mI] = mV[(mOpcode & 0x0F00) >> 8] / 100;
                    mMemory[mI + 1] = (mV[(mOpcode & 0x0F00) >> 8] / 10) % 10;
                    mMemory[mI + 2] = (mV[(mOpcode & 0x0F00) >> 8] % 100) % 10;
                    mProgramCounter += 2;
                    break;

                case 0x0055:
                    for (int i = 0; i <= ((mOpcode & 0x0F00) >> 8); ++i)
                        mMemory[mI + i] = mV[i];

                    // On the original interpreter, when the operation is done, I = I + X + 1.
                    mI += ((mOpcode & 0x0F00) >> 8) + 1;
                    mProgramCounter += 2;
                    break;

                case 0x065:
                    for (int i = 0; i <= ((mOpcode & 0x0F00) >> 8); ++i)
                        mV[i] = mMemory[mI + i];

                    // On the original interpreter, when the operation is done, I = I + X + 1.
                    mI += ((mOpcode & 0x0F00) >> 8) + 1;
                    mProgramCounter += 2;


                    break;

            }
        }
        break;






        default:

            std::cout << "Unknown opcode " << std::hex << mOpcode << '\n';
    }

    // Update timers
    if (mDelayTimer > 0)
        --mDelayTimer;

    if (mSoundTimer > 0)
    {
        if (mSoundTimer == 1)
            printf("BEEP!\n");
        --mSoundTimer;
    }

    // Update Timers
}

void CPU::LoadGame(const std::string& game)
{

    std::ifstream input(game, std::ios::in | std::ios::binary);
    char c;
    int i = 0;
    while (input.get(c))
    {
        mMemory[i + 512] = c;
        ++i;
    }
}
