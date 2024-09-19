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

You control the yellow square. Use the W, A, S, and D keys to go up, left, down, and right, respectively. Red squares are fruit. You can eat fruit by moving over them. They don't do anything... sadly. But we got bombs! Press the spacebar to throw a bomb that explodes when it hits a wall. An FPS counter in the top-right corner tells you how the game is performing on your system. Framerate is capped at 60 fps for visual fidelity.

Reach the highest level you can!
