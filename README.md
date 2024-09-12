# Pac-Man with Bombs!

## How to Compile on macOS
The easiest way to compile this project on macOS is with [Homebrew] and [CMake].

1. [Install Homebrew] if you have not already.
1. Install CMake, [SDL2], [SDL2_ttf], and [OpenSSL] with Homebrew:
   ```
   brew install cmake sdl2 sdl2_ttf openssl
   ```
1. Download this repository and `cd` into it.
1. Run CMake:
   ```
   cmake -S . -B build && cmake --build build
   ```

## How to Play
1. Run the executable in `bin`:
   ```
   ./bin/main
   ```

You control the yellow square. Use the W, A, S, and D keys to go up, left, down, and right, respectively. Red squares are fruit. You can eat fruit by moving over them. An FPS counter in the top-right corner tells you how the game is performing on your system; the higher the number, the better.

## Todo - Jozef
1. Add bombs!
1. Make walls block movement (border walls currently do this; the goal is for wall tiles to block player movement as well)
1. Make the map more like a maze than an empty grid
1. Add enemies

## Todo - Matt
1. Make a HUD
1. Create a goal for the player to strive for, whether it’s with points, levels, survival time, or a combination of the aforementioned systems
1. Add sound
1. Make the fruit do something

[CMake]: https://cmake.org/
[Homebrew]: https://brew.sh/
[Install Homebrew]: https://docs.brew.sh/Installation
[SDL2]: https://github.com/libsdl-org/SDL/tree/SDL2
[SDL2_ttf]: https://github.com/libsdl-org/SDL_ttf/tree/SDL2
[OpenSSL]: https://github.com/openssl/openssl
