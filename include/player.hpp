#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "bomb.hpp"
#include "direction.hpp"
#include <forward_list>
#include "highest_resolution_steady_clock.hpp"
#include "keyboard.hpp"
#include <list>
#include "point.hpp"
#include <queue>
#include "tile.hpp"
#include <SDL.h>

class Player {
public:
    Player(
        SDL_Window* _window,
        SDL_Renderer* _renderer,
        int _numRows,
        int _numCols,
        int _tileSize,
        SDL_Point _gridOffset,
        SDL_Point _position,
        double _speed,
        Direction _direction
    );

    SDL_Point get_position() const;
    SDL_Point get_next_position() const;
    bool check_collision();
    void set_direction(const Keyboard _keyboard, SDL_Keycode _key);
    void collided_with_wall(const bool turned, const SDL_Point prevPos);
    int move();
    void draw();
    void reset(SDL_Point _position, double _speed, Direction _direction);
    SDL_Rect get_rect() const;
    std::vector<Bomb> bombs;
    Direction direction;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    const int numRows;
    const int numCols;
    int tileSize;
    SDL_Point gridOffset;
    SDL_Point position;
    double speed;
    double offset;
    std::forward_list<SDL_Keycode> keyBuffer;
    std::queue<Direction> turnBuffer;
    std::chrono::time_point<highest_resolution_steady_clock> prevTime;
    bool prevTimeValid;
};

#endif
