#include "Platform.h"
#include "Chip8.h"
#include <optional>

Platform::Platform()
    : window(sf::VideoMode({ 640, 320 }), "CHIP-8 Emulator"),
    image(sf::Vector2u{ 64, 32 }, sf::Color::Black),
    sprite(texture)
{
    window.setFramerateLimit(60);

    // Сначала заполняем текстуру реальным содержимым
    texture.loadFromImage(image);         // вместо resize + update

    // Теперь связываем спрайт с готовой текстурой и сбрасываем его прямоугольник
    sprite.setTexture(texture, true);     // true = resetRect
    sprite.setScale({ 10.f, 10.f });
}

bool Platform::ProcessInput(Chip8& chip8) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return false;
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window.close();
                return false;
            }

            // Маппинг клавиш ПК → CHIP-8
            switch (keyPressed->code) {
                // Верхний ряд: 1 2 3 4  →  0x1 0x2 0x3 0xC
            case sf::Keyboard::Key::Num1: chip8.SetKey(0x1, true); break;
            case sf::Keyboard::Key::Num2: chip8.SetKey(0x2, true); break;
            case sf::Keyboard::Key::Num3: chip8.SetKey(0x3, true); break;
            case sf::Keyboard::Key::Num4: chip8.SetKey(0xC, true); break;

                // Второй ряд: Q W E R  →  0x4 0x5 0x6 0xD
            case sf::Keyboard::Key::Q:    chip8.SetKey(0x4, true); break;
            case sf::Keyboard::Key::W:    chip8.SetKey(0x5, true); break;
            case sf::Keyboard::Key::E:    chip8.SetKey(0x6, true); break;
            case sf::Keyboard::Key::R:    chip8.SetKey(0xD, true); break;

                // Третий ряд: A S D F  →  0x7 0x8 0x9 0xE
            case sf::Keyboard::Key::A:    chip8.SetKey(0x7, true); break;
            case sf::Keyboard::Key::S:    chip8.SetKey(0x8, true); break;
            case sf::Keyboard::Key::D:    chip8.SetKey(0x9, true); break;
            case sf::Keyboard::Key::F:    chip8.SetKey(0xE, true); break;

                // Нижний ряд: Z X C V  →  0xA 0x0 0xB 0xF
            case sf::Keyboard::Key::Z:    chip8.SetKey(0xA, true); break;
            case sf::Keyboard::Key::X:    chip8.SetKey(0x0, true); break;
            case sf::Keyboard::Key::C:    chip8.SetKey(0xB, true); break;
            case sf::Keyboard::Key::V:    chip8.SetKey(0xF, true); break;

            default: break;
            }
        }

        if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            switch (keyReleased->code) {
            case sf::Keyboard::Key::Num1: chip8.SetKey(0x1, false); break;
            case sf::Keyboard::Key::Num2: chip8.SetKey(0x2, false); break;
            case sf::Keyboard::Key::Num3: chip8.SetKey(0x3, false); break;
            case sf::Keyboard::Key::Num4: chip8.SetKey(0xC, false); break;

            case sf::Keyboard::Key::Q:    chip8.SetKey(0x4, false); break;
            case sf::Keyboard::Key::W:    chip8.SetKey(0x5, false); break;
            case sf::Keyboard::Key::E:    chip8.SetKey(0x6, false); break;
            case sf::Keyboard::Key::R:    chip8.SetKey(0xD, false); break;

            case sf::Keyboard::Key::A:    chip8.SetKey(0x7, false); break;
            case sf::Keyboard::Key::S:    chip8.SetKey(0x8, false); break;
            case sf::Keyboard::Key::D:    chip8.SetKey(0x9, false); break;
            case sf::Keyboard::Key::F:    chip8.SetKey(0xE, false); break;

            case sf::Keyboard::Key::Z:    chip8.SetKey(0xA, false); break;
            case sf::Keyboard::Key::X:    chip8.SetKey(0x0, false); break;
            case sf::Keyboard::Key::C:    chip8.SetKey(0xB, false); break;
            case sf::Keyboard::Key::V:    chip8.SetKey(0xF, false); break;

            default: break;
            }
        }
    }
    return window.isOpen();
}

void Platform::Update(const uint32_t* videoBuffer) {
    for (unsigned y = 0; y < 32; ++y) {
        for (unsigned x = 0; x < 64; ++x) {
            uint32_t pixel = videoBuffer[y * 64 + x];
            image.setPixel({ x, y }, pixel ? sf::Color::White : sf::Color::Black);
        }
    }
    texture.loadFromImage(image);   // <-- было update(image)
}

void Platform::Render() {
    window.clear(sf::Color::Black);
    window.draw(sprite);
    window.display();
}