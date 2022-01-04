#pragma once
#include <array>
#include <cstdint>
#include <string>

class CPU
{

    uint16_t mOpcode;

    /*
    0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
    0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
    0x200-0xFFF - Program ROM and work RAM
    */

    std::array<uint8_t, 4096> mMemory;
    std::array<uint8_t, 16> mV; // Registers

    uint16_t mI;  // Index register
    uint16_t mProgramCounter; // Program counter

    std::array<std::array<uint16_t, 32>, 64 > mGraphics;

    uint8_t mDelayTimer;
    uint8_t mSoundTimer;

    std::array< uint16_t, 16> mStack;
    uint16_t mStackPointer;


    public:
    uint8_t mKey;

    void Initialize();
    void EmulateCycle();
    void LoadGame(const std::string& game);

    bool DrawFlag;

    inline std::array<std::array<uint16_t, 32>, 64 > GetGraphics() { return mGraphics; }
    //  
};
