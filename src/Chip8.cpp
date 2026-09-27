#include "Chip8.h"
#include <fstream>
#include <iostream>
#include <cstring>

// Стандартный шрифт CHIP-8: каждая цифра/буква — 5 байт (5 строк по 4 пикселя)
static const uint8_t FONTSET[80] = {
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

Chip8::Chip8() {
    pc = 0x200;
    opcode = 0;
    index = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;
    LoadFontset();
}

void Chip8::LoadFontset() {
    for (int i = 0; i < 80; ++i) {
        memory[i] = FONTSET[i];
    }
}

bool Chip8::LoadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open ROM: " << filename << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > (4096 - 512)) {
        std::cerr << "ROM too large to fit in memory" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(memory + 0x200), size);
    file.close();

    std::cout << "Loaded ROM: " << filename << " (" << size << " bytes)" << std::endl;
    return true;
}   // <---- ВОТ ЗДЕСЬ ЗАКРЫВАЕТСЯ LoadROM. Всё, что ниже — снаружи.

// ============================================================
// Всё, что ниже, должно быть ВНЕ функции LoadROM!
// ============================================================

void Chip8::UpdateTimers() {
    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}

void Chip8::EmulateCycle() {
    // FETCH
    opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    // DECODE & EXECUTE
    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode & 0x00FF) {
        case 0x00E0: // CLS — очистить экран
            for (int i = 0; i < 64 * 32; ++i) video[i] = 0;
            drawFlag = true;
            break;
        case 0x00EE: // RET — возврат из подпрограммы
            --sp;
            pc = stack[sp];
            break;
        default:
            std::cerr << "Unknown 0NNN: 0x" << std::hex << opcode << std::endl;
        }
        break;

    case 0x1000: // JP addr
        pc = opcode & 0x0FFF;
        break;

    case 0x2000: // CALL addr
        stack[sp] = pc;
        ++sp;
        pc = opcode & 0x0FFF;
        break;

    case 0x3000: { // SE Vx, byte
        uint8_t X = (opcode & 0x0F00) >> 8;
        if (registers[X] == (opcode & 0x00FF)) pc += 2;
        break;
    }

    case 0x4000: { // SNE Vx, byte
        uint8_t X = (opcode & 0x0F00) >> 8;
        if (registers[X] != (opcode & 0x00FF)) pc += 2;
        break;
    }

    case 0x5000: { // SE Vx, Vy
        uint8_t X = (opcode & 0x0F00) >> 8;
        uint8_t Y = (opcode & 0x00F0) >> 4;
        if (registers[X] == registers[Y]) pc += 2;
        break;
    }

    case 0x6000: { // LD Vx, byte
        uint8_t X = (opcode & 0x0F00) >> 8;
        registers[X] = opcode & 0x00FF;
        break;
    }

    case 0x7000: { // ADD Vx, byte
        uint8_t X = (opcode & 0x0F00) >> 8;
        registers[X] += opcode & 0x00FF;
        break;
    }

    case 0x8000: {
        uint8_t X = (opcode & 0x0F00) >> 8;
        uint8_t Y = (opcode & 0x00F0) >> 4;
        switch (opcode & 0x000F) {
        case 0x0: registers[X] = registers[Y]; break;
        case 0x1: registers[X] |= registers[Y]; break;
        case 0x2: registers[X] &= registers[Y]; break;
        case 0x3: registers[X] ^= registers[Y]; break;
        case 0x4: { // ADD с флагом переноса
            uint16_t sum = registers[X] + registers[Y];
            registers[0xF] = (sum > 255) ? 1 : 0;
            registers[X] = sum & 0xFF;
            break;
        }
        case 0x5: { // SUB
            registers[0xF] = (registers[X] > registers[Y]) ? 1 : 0;
            registers[X] -= registers[Y];
            break;
        }
        case 0x6: { // SHR
            registers[0xF] = registers[X] & 0x1;
            registers[X] >>= 1;
            break;
        }
        case 0x7: { // SUBN
            registers[0xF] = (registers[Y] > registers[X]) ? 1 : 0;
            registers[X] = registers[Y] - registers[X];
            break;
        }
        case 0xE: { // SHL
            registers[0xF] = (registers[X] >> 7) & 0x1;
            registers[X] <<= 1;
            break;
        }
        default:
            std::cerr << "Unknown 8XY: 0x" << std::hex << opcode << std::endl;
        }
        break;
    }

    case 0x9000: { // SNE Vx, Vy
        uint8_t X = (opcode & 0x0F00) >> 8;
        uint8_t Y = (opcode & 0x00F0) >> 4;
        if (registers[X] != registers[Y]) pc += 2;
        break;
    }

    case 0xA000: // LD I, addr
        index = opcode & 0x0FFF;
        break;

    case 0xB000: // JP V0, addr
        pc = (opcode & 0x0FFF) + registers[0];
        break;

    case 0xC000: { // RND Vx, byte
        uint8_t X = (opcode & 0x0F00) >> 8;
        registers[X] = (std::rand() % 256) & (opcode & 0x00FF);
        break;
    }

    case 0xD000: { // DRW Vx, Vy, nibble — рисование спрайта
        uint8_t X = (opcode & 0x0F00) >> 8;
        uint8_t Y = (opcode & 0x00F0) >> 4;
        uint8_t height = opcode & 0x000F;
        uint8_t xPos = registers[X] % 64;
        uint8_t yPos = registers[Y] % 32;
        registers[0xF] = 0;

        for (int row = 0; row < height; ++row) {
            uint8_t spriteByte = memory[index + row];
            for (int col = 0; col < 8; ++col) {
                if (spriteByte & (0x80 >> col)) {
                    int px = (xPos + col) % 64;
                    int py = (yPos + row) % 32;
                    if (video[py * 64 + px]) registers[0xF] = 1;
                    video[py * 64 + px] ^= 1;
                }
            }
        }
        drawFlag = true;
        break;
    }

    case 0xE000: {
        uint8_t X = (opcode & 0x0F00) >> 8;
        switch (opcode & 0x00FF) {
        case 0x9E: if (keypad[registers[X]]) pc += 2; break;
        case 0xA1: if (!keypad[registers[X]]) pc += 2; break;
        default:
            std::cerr << "Unknown EX: 0x" << std::hex << opcode << std::endl;
        }
        break;
    }

    case 0xF000: {
        uint8_t X = (opcode & 0x0F00) >> 8;
        switch (opcode & 0x00FF) {
        case 0x07: registers[X] = delayTimer; break;
        case 0x0A: { // Ждать нажатия клавиши
            bool pressed = false;
            for (int i = 0; i < 16; ++i) {
                if (keypad[i]) {
                    registers[X] = i;
                    pressed = true;
                    break;
                }
            }
            if (!pressed) pc -= 2;
            break;
        }
        case 0x15: delayTimer = registers[X]; break;
        case 0x18: soundTimer = registers[X]; break;
        case 0x1E: index += registers[X]; break;
        case 0x29: index = registers[X] * 5; break; // адрес шрифта
        case 0x33: { // BCD — три цифры
            memory[index] = registers[X] / 100;
            memory[index + 1] = (registers[X] / 10) % 10;
            memory[index + 2] = registers[X] % 10;
            break;
        }
        case 0x55: { // LD [I], Vx
            for (int i = 0; i <= X; ++i) memory[index + i] = registers[i];
            break;
        }
        case 0x65: { // LD Vx, [I]
            for (int i = 0; i <= X; ++i) registers[i] = memory[index + i];
            break;
        }
        default:
            std::cerr << "Unknown FX: 0x" << std::hex << opcode << std::endl;
        }
        break;
    }

    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
    }
}