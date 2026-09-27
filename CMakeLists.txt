cmake_minimum_required(VERSION 3.20)
project(Chip8Emulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Явно указываем, где искать SFML (на всякий случай)
set(SFML_DIR "C:/dev/vcpkg/installed/x64-windows/share/sfml")

# Синтаксис для SFML 3: компоненты с большой буквы, CONFIG REQUIRED
find_package(SFML COMPONENTS System Window Graphics CONFIG REQUIRED)

add_executable(Chip8Emulator
    src/main.cpp
    src/Chip8.cpp
    src/Platform.cpp
)

# Линкуем с целями из пространства имён SFML::
target_link_libraries(Chip8Emulator PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

set_target_properties(Chip8Emulator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)