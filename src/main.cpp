#include "Chip8.h"
#include "Platform.h"
#include <iostream>
#include <chrono>

int main() {
    std::string romPath = "C:/dev/chip8-emulator/roms/Pong (1 player).ch8";
    Chip8 chip8;
    if (!chip8.LoadROM(romPath)) return 1;

    Platform platform;

    // Настоящий CHIP-8 работает на ~500-700 Гц.
    // При 60 FPS это ~10 инструкций на кадр.
    const int CYCLES_PER_FRAME = 10;

    auto lastTimerUpdate = std::chrono::steady_clock::now();

    while (platform.IsOpen()) {
        if (!platform.ProcessInput(chip8)) break;

        // Выполняем фиксированное число инструкций за кадр
        for (int i = 0; i < CYCLES_PER_FRAME; ++i) {
            chip8.EmulateCycle();
        }

        // Таймеры уменьшаются 60 раз в секунду
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastTimerUpdate).count() >= 16) {
            chip8.UpdateTimers();
            lastTimerUpdate = now;
        }

        if (chip8.drawFlag) {
            platform.Update(chip8.GetVideoBuffer());
            chip8.drawFlag = false;
        }

        platform.Render();  // ограничено 60 FPS через setFramerateLimit
    }

    return 0;
}