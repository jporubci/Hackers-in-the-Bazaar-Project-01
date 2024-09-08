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

You control the yellow square. Use the W, A, S, and D keys to go up, left, down, and right, respectively. It is game over if you run into the walls. Red squares are fruit. You can eat fruit by moving over them. An FPS counter in the top-right corner tells you how the game is performing on your system; the higher the number, the better.

## Todo
- Fix movement so that it is possible to stand still.
- Add enemies that follow the player.
- Make the fruit do something.
- Make the map more like a maze than an empty grid.
- Prevent the player from moving through a wall instead of ending the game when a player moves through a wall.
- Add some HUD elements.
- Add bombs!

[CMake]: https://cmake.org/
[Homebrew]: https://brew.sh/
[Install Homebrew]: https://docs.brew.sh/Installation
[SDL2]: https://github.com/libsdl-org/SDL/tree/SDL2
[SDL2_ttf]: https://github.com/libsdl-org/SDL_ttf/tree/SDL2
[OpenSSL]: https://github.com/openssl/openssl
