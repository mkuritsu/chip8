#include "chip8.hpp"

#include <cstring>
#include <iostream>

namespace chip8
{
    void Registers::clear()
    {
        std::memset(v.data(), 0, v.size());
        pc = 0x200;
        i = 0;
        sp = 0;
    }

    void Display::clear()
    {
        std::memset(pixels.data(), false, pixels.size());
    }

    void Chip8::loadROM(const std::vector<uint8_t>& rom)
    {
        reset();
        std::memcpy(m_RAM.data() + m_Registers.pc, rom.data(), rom.size());
    }

    void Chip8::reset()
    {
        m_Registers.clear();
        m_Display.clear();
        std::memset(m_RAM.data(), 0, m_RAM.size());
        std::memset(m_Stack.data(), 0, m_Stack.size());
        std::memset(m_Keyboard.data(), false, m_Keyboard.size());
        srand(time(NULL));
    }

    void Chip8::runCycle()
    {
        uint16_t opcode = m_RAM[m_Registers.pc] << 8 | m_RAM[m_Registers.pc + 1];
        switch (opcode & 0xF000)
        {
        case 0x0000:
            switch (opcode & 0xFF)
            {
            case 0xE0: // CLS
                m_Display.clear();
                break;
            case 0xEE: // RET
                m_Registers.pc = m_Stack[m_Registers.sp--];
                break;
            default: // SYS addr - ignored
                break;
            }
            break;
        case 0x1000: // JMP addr
            m_Registers.pc = opcode & 0xFFF;
            break;
        case 0x2000: // CALL addr
            m_Stack[++m_Registers.sp] = m_Registers.pc;
            m_Registers.pc = opcode & 0xFFF;
            break;
        case 0x3000: { // SE Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 1;
            uint16_t value = opcode & 0xFF;
            m_Registers.pc += m_Registers.v[x] == value ? 4 : 2;
            break;
        }
        case 0x4000: { // SNE Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 1;
            uint16_t value = opcode & 0xFF;
            m_Registers.pc += m_Registers.v[x] != value ? 4 : 2;
            break;
        }
        case 0x5000: { // SE Vx, Vy
            uint8_t x = (opcode & 0x0F00) >> 1;
            uint8_t y = (opcode & 0xF0) - 0xF;
            m_Registers.pc += m_Registers.v[x] == m_Registers.v[y] ? 4 : 2;
            break;
        }
        case 0x6000: { // LD Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 1;
            uint16_t value = opcode & 0xFF;
            m_Registers.v[x] = value;
            m_Registers.pc += 2;
            break;
        }
        case 0x7000: { // ADD Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 1;
            uint16_t value = opcode & 0xFF;
            m_Registers.v[x] += value;
            m_Registers.pc += 2;
            break;
        }
        case 0x8000: {
            uint8_t x = (opcode & 0x0F00) >> 1;
            uint8_t y = (opcode & 0xF0) - 0xF;
            switch (opcode & 0xF)
            {
            case 0x0: // LD Vx, Vy
                m_Registers.v[x] = m_Registers.v[y];
                break;
            case 0x1: // OR Vx, Vy
                m_Registers.v[x] |= m_Registers.v[y];
                break;
            case 0x2: // AND Vx, Vy
                m_Registers.v[x] &= m_Registers.v[y];
                break;
            case 0x3: // XOR Vx, Vy
                m_Registers.v[x] ^= m_Registers.v[y];
                break;
            case 0x4: // ADD Vx, Vy
            case 0x5: // SUB Vx, Vy
            case 0x6: // SHR Vx, Vy
            case 0x7: // SUBN Vx, Vy
            case 0xE: // SHL Vx, Vy
            default:
                std::cerr << "Invalid instruction: " << std::hex << opcode << std::endl;
                std::abort();
            }
            break;
            m_Registers.pc += 2;
            break;
        }
        case 0x9000: { // SNE Vx, Vy
            uint8_t x = (opcode & 0x0F00) >> 1;
            uint8_t y = (opcode & 0xF0) - 0xF;
            m_Registers.pc += m_Registers.v[x] != m_Registers.v[y] ? 4 : 2;
            break;
        }
        case 0xA000: // LD I, addr
            m_Registers.i = opcode & 0xFFF;
            m_Registers.pc += 2;
            break;
        case 0xB000: // JP V0, addr
            m_Registers.pc = (opcode & 0xFFF) + m_Registers.v[0];
            break;
        case 0xC000: { // RND Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 1;
            uint16_t value = opcode & 0xFF;
            m_Registers.v[x] = static_cast<uint8_t>(rand() % 255) & value;
            m_Registers.pc += 2;
            break;
        }
        case 0xD000: { // DRW Vx, Vy, nibble
        }
        case 0xE000: {
            uint8_t x = (opcode & 0x0F00) >> 1;
            switch (opcode & 0xFF)
            {
            case 0x9E: // SKP Vx
                m_Registers.pc += m_Keyboard[x] ? 4 : 2;
                break;
            case 0xA1: // SKNP Vx
                m_Registers.pc += m_Keyboard[x] ? 2 : 4;
                break;
            }
            break;
        }
        case 0xF000: {
            uint8_t x = (opcode & 0x0F00) >> 1;
            switch (opcode & 0xFF)
            {
            case 0x07: // LD Vx, DT
                m_Registers.v[x] = m_Timers.delay;
                break;
            case 0x0A: // LD Vx, K
                // TODO: wait for key press
                break;
            case 0x15: // LD DT, Vx
                m_Timers.delay = m_Registers.v[x];
                break;
            case 0x18: // LD ST, Vx
                m_Timers.sound = m_Registers.v[x];
                break;
            case 0x1E: // ADD I, Vx
                m_Registers.i += m_Registers.v[x];
                break;
            case 0x29: // LD F, Vx
                // TODO
                break;
            case 0x33: // LD B, Vx
                // TODO
                break;
            case 0x55: // LD [I], Vx
                // TODO
                break;
            case 0x65: // LD Vx, [I]
                break;
            }
            m_Registers.pc += 2;
            break;
        }
        default:
            std::cerr << "Invalid instruction: " << std::hex << opcode << std::endl;
            std::abort();
        }
    }
} // namespace chip8