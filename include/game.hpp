#ifndef GAME_HPP
#define GAME_HPP

#include "enemy.hpp"
#include "keyboard.hpp"
#include "grid.hpp"
#include "player.hpp"
#include "hud.hpp"
#include <SDL.h>

enum class State;

class Game {
public:
    Game(
        SDL_Window* _window,
        SDL_Renderer* _renderer
    );
    
    bool gameOn() const;
    void poll();
    void step();
    void draw();

private:
    enum class State {
        quitGame,
        newGame,
        playGame,
        gameOver,
    };

    SDL_Window* window;
    SDL_Renderer* renderer;
    State state;
    Keyboard keyboard;
    int numRows;
    int numCols;
    int tileSize;
    Grid grid;
    Player player;
    std::vector<Enemy> enemies;
    HUD hud;
};

#endif
