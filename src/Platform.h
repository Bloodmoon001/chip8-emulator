#pragma once

#include <SFML/Graphics.hpp>

class Chip8;  // forward declaration

class Platform {
public:
    Platform();

    bool ProcessInput(Chip8& chip8);
    void Update(const uint32_t* videoBuffer);
    void Render();
    bool IsOpen() const { return window.isOpen(); }

private:
    sf::RenderWindow window;
    sf::Texture      texture;
    sf::Image        image;
    sf::Sprite       sprite;   // <-- без инициализатора, инициализируем в конструкторе
};