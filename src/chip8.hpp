#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace chip8
{
    struct Registers
    {
        std::array<uint8_t, 16> v;
        uint16_t pc{0x200};
        uint16_t i{0};
        uint8_t sp{0};

        void clear();
    };

    struct Display
    {
        std::array<bool, 64 * 32> pixels;

        void clear();
    };

    struct Timers
    {
        uint8_t sound{0};
        uint8_t delay{0};
    };

    class Chip8
    {
    public:
        void loadROM(const std::vector<uint8_t>& rom);

        void reset();

        void runCycle();

    private:
        Display m_Display;
        Registers m_Registers;
        Timers m_Timers;
        std::array<uint8_t, 4096> m_RAM;
        std::array<uint16_t, 16> m_Stack;
        std::array<bool, 16> m_Keyboard;
    };
} // namespace chip8