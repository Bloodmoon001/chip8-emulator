# CHIP-8 Emulator

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg)](https://cmake.org/)
[![SFML](https://img.shields.io/badge/SFML-3.0-orange.svg)](https://www.sfml-dev.org/)

A fully functional CHIP-8 emulator written in C++17, built with CMake and SFML 3.

Полнофункциональный эмулятор CHIP-8, написанный на C++17 с использованием CMake и SFML 3.

---

## 🇬🇧 English

### About The Project
This project is a complete implementation of a CHIP-8 virtual machine. It can load and run classic CHIP-8 ROMs, handling graphics, input, and timers. The goal was to build a clean, well-structured emulator from scratch, demonstrating a solid understanding of low-level programming concepts.

### Features
* **Full Instruction Set:** Implements all 35 opcodes of the CHIP-8 instruction set.
* **Graphics:** Renders the 64x32 pixel display using SFML.
* **Input:** Supports the standard 16-key keypad mapping for gameplay.
* **Timers:** Includes accurate delay and sound timer emulation.
* **Clean Architecture:** Separates the core emulator logic (`Chip8` class) from the platform-specific rendering (`Platform` class).

### Built With
* **C++17**
* **CMake** (for build management)
* **SFML 3** (for graphics, window, and input)
* **vcpkg** (for dependency management)

### Getting Started
Follow these steps to get a local copy up and running.

#### Prerequisites
* **Visual Studio 2022** (with "Desktop development with C++" workload)
* **CMake** (version 3.20 or higher)
* **vcpkg** (for installing SFML)

#### Installation & Build
1.  **Install vcpkg** (if you haven't already):
    ```bash
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.bat
    ./vcpkg integrate install