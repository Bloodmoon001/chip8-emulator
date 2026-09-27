#pragma once

#include <cstdint>
#include <string>

class Chip8 {
public:
    Chip8();

    // Загрузка ROM-файла в память, начиная с адреса 0x200
    bool LoadROM(const std::string& filename);

    // Один цикл: fetch → decode → execute
    void EmulateCycle();

    // Уменьшение таймеров (вызывать 60 раз в секунду)
    void UpdateTimers();

    // Доступ к состоянию (для рендера и ввода)
    const uint32_t* GetVideoBuffer() const { return video; }
    void SetKey(uint8_t index, bool pressed) { keypad[index] = pressed ? 1 : 0; }
    bool IsSoundActive() const { return soundTimer > 0; }

    // Флаг, что экран изменился и нужно перерисовать
    bool drawFlag = false;

private:
    // Память и регистры
    uint8_t  memory[4096]{};      // 4 КБ RAM
    uint8_t  registers[16]{};     // V0–VF
    uint16_t index{};             // Регистр I
    uint16_t pc{};                // Program Counter
    uint16_t stack[16]{};         // Стек вызовов
    uint8_t  sp{};                // Stack Pointer
    uint8_t  delayTimer{};        // Таймер задержки
    uint8_t  soundTimer{};        // Звуковой таймер
    uint8_t  keypad[16]{};        // Состояние клавиш
    uint32_t video[64 * 32]{};    // Буфер дисплея (0 = чёрный, 1 = белый)
    uint16_t opcode{};            // Текущий опкод

    // Инициализация шрифта в памяти (0x000–0x1FF)
    void LoadFontset();
};