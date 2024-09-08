#ifndef SCENE_HPP
#define SCENE_HPP

#include "fpscounter.hpp"
#include "game.hpp"
#include "highest_resolution_steady_clock.hpp"
#include <SDL.h>

class Scene {
public:
    Scene();
    ~Scene();
    void run();

private:
    SDL_Window* init_window();
    SDL_Renderer* init_renderer();

    void clear_frame();
    void display_frame();
    void delay_frame();

    const char* windowName;
    bool vsyncOn;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::chrono::time_point<highest_resolution_steady_clock> prevTime;
    bool prevTimeValid;
    double maxRefreshRate;
    FPSCounter fpsCounter;
    Game game;
};

#endif
