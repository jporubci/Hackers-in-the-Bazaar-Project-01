#include "scene.hpp"
#include <cstdlib>

Scene::Scene() :
    windowName("Pac-Man with Bombs!"),
    vsyncOn(false),
    window(init_window()),
    renderer(init_renderer()),
    prevTime(),
    prevTimeValid(false),
    maxRefreshRate(0.0),
    fpsCounter(window, renderer),
    game(window, renderer)
{}

SDL_Window* Scene::init_window() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Window* _window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (_window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return _window;
}

SDL_Renderer* Scene::init_renderer() {
    Uint32 _rendererFlags = SDL_RENDERER_ACCELERATED;
    if (vsyncOn) {
        _rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }
    SDL_Renderer* _renderer = SDL_CreateRenderer(window, -1, _rendererFlags);
    if (_renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return _renderer;
}

void Scene::run() {
    while (game.gameOn()) {
        clear_frame();
        game.poll();
        game.step();
        game.draw();
        fpsCounter.draw();
        display_frame();
        delay_frame();
    }
}

void Scene::clear_frame() {
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (SDL_RenderClear(renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_RenderClear() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void Scene::display_frame() {
    SDL_RenderPresent(renderer);
}

void Scene::delay_frame() {
    if (maxRefreshRate <= 0.0) {
        return;
    }

    if (!prevTimeValid) {
        prevTime = highest_resolution_steady_clock::now();
        prevTimeValid = true;
        return;
    }

    const auto _interval = std::chrono::duration_cast<std::chrono::nanoseconds>(highest_resolution_steady_clock::now() - prevTime).count();
    if (_interval < 1e9 / maxRefreshRate) {
        SDL_Delay((1e9 / maxRefreshRate - _interval) / 1e6);
    }

    prevTime = highest_resolution_steady_clock::now();
}

Scene::~Scene() {
    fpsCounter.shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
