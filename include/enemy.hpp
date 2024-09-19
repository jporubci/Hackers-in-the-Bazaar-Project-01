#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "direction.hpp"
#include "grid.hpp"
#include "highest_resolution_steady_clock.hpp"
#include <list>
#include "point.hpp"
#include <queue>
#include "tile.hpp"
#include <SDL.h>

class Enemy {
public:
    Enemy(
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
    bool check_collision(const SDL_Rect& playerRect);
    void set_direction();
    void collided_with_wall(const bool turned, const SDL_Point prevPos);
    void move(const std::vector<std::vector<Tile>>& grid);
    void draw();
    void reset(SDL_Point _position, double _speed, Direction _direction);
    SDL_Rect get_rect() const;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    int numRows;
    int numCols;
    int tileSize;
    SDL_Point gridOffset;
    SDL_Point position;
    double speed;
    Direction direction;
    double offset;
    std::chrono::time_point<highest_resolution_steady_clock> prevTime;
    bool prevTimeValid;
};

#endif
