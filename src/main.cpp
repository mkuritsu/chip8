#include "chip8.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage " << argv[0] << " <rom file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream romFile{argv[1]};
    if (!romFile.is_open())
    {
        std::cerr << "Failed to open ROM file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }
    std::vector<uint8_t> rom{std::istreambuf_iterator<char>(romFile), std::istreambuf_iterator<char>()};

    chip8::Chip8 chip8;
    chip8.loadROM(rom);
    std::cout << "ROM LOADED!" << std::endl;
    while (true)
    {
        chip8.runCycle();
    }
    return 0;
}