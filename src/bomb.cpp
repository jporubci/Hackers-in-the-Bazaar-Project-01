#include "bomb.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <cstdlib>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "tile.hpp"

Bomb::Bomb(
    SDL_Window* _window,
    SDL_Renderer* _renderer,
    int _numRows,
    int _numCols,
    int _tileSize,
    SDL_Point _gridOffset,
    SDL_Point _position,
    double _speed,
    Direction _direction
) :
    window(_window),
    renderer(_renderer),
    numRows(_numRows),
    numCols(_numCols),
    tileSize(_tileSize),
    gridOffset(_gridOffset),
    position(_position),
    speed(_speed),
    direction(_direction),
    offset(tileSize - 0.01),
    prevTime(),
    prevTimeValid(false),
    exploding(false),
    lifetime(0.2)
{}

SDL_Point Bomb::get_next_position() const {
    SDL_Point nextPosition(position);

    switch (direction) {
        case Direction::up:
            nextPosition.y = std::max(0, nextPosition.y - 1);
            break;
        case Direction::down:
            nextPosition.y = std::min(numRows - 1, nextPosition.y + 1);
            break;
        case Direction::left:
            nextPosition.x = std::max(0, nextPosition.x - 1);
            break;
        case Direction::right:
            nextPosition.x = std::min(numCols - 1, nextPosition.x + 1);
            break;
        default:
            break;
    }

    return nextPosition;
}

bool Bomb::check_collision(const std::vector<std::vector<Tile>>& grid) {
    SDL_Point nextPos = get_next_position();
    return grid.at(nextPos.y).at(nextPos.x) == Tile::wall;
}

SDL_Rect Bomb::explode() {
    auto _currTime = highest_resolution_steady_clock::now();
    SDL_Rect explosion = get_rect();
    explosion.x -= tileSize;
    explosion.y -= tileSize;
    explosion.h *= 3;
    explosion.w = explosion.h;
    if (!exploding) {
        if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        if (SDL_RenderClear(renderer) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_RenderClear() failed: %s", SDL_GetError());
            exit(EXIT_FAILURE);
        }
        exploding = true;
        explosion.h = 0;
        prevTime = _currTime;
        return explosion;
    }
    auto _interval = std::chrono::duration_cast<std::chrono::nanoseconds>(_currTime - prevTime).count();
    if (_interval <= 0) {
        return explosion;
    }
    lifetime -= _interval / 1e9;
    prevTime = _currTime;
    if (lifetime > 0) {
        return explosion;
    }
    explosion.h = 0;
    return explosion;
}

void Bomb::move() {
    if (exploding) {
        return;
    }

    auto _currTime = highest_resolution_steady_clock::now();
    if (!prevTimeValid) {
        prevTime = _currTime;
        prevTimeValid = true;
        return;
    }

    /* Check if any time has passed */
    auto _interval = std::chrono::duration_cast<std::chrono::nanoseconds>(_currTime - prevTime).count();
    if (_interval <= 0) {
        return;
    }

    /* Move the bomb */
    offset += speed / (1e9 / _interval);
    prevTime = _currTime;

    switch (direction) {
        case Direction::up:
            position.y -= static_cast<int>(offset) / tileSize;
            if (position.y <= 0) {
                position.y = 0;
                offset = 0;
            }
            break;
        case Direction::down:
            position.y += static_cast<int>(offset) / tileSize;
            if (position.y >= numRows - 1) {
                position.y = numRows - 1;
                offset = 0;
            }
            break;
        case Direction::left:
            position.x -= static_cast<int>(offset) / tileSize;
            if (position.x <= 0) {
                position.x = 0;
                offset = 0;
            }
            break;
        case Direction::right:
            position.x += static_cast<int>(offset) / tileSize;
            if (position.x >= numCols - 1) {
                position.x = numCols - 1;
                offset = 0;
            }
            break;
        default:
            break;
    }

    if (offset >= tileSize) {
        offset = fmod(offset, tileSize);
    }
}

void Bomb::draw() {
    if (SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Rect bombRect = get_rect();

    SDL_RenderFillRect(renderer, &bombRect);
}

SDL_Rect Bomb::get_rect() const {
    SDL_Rect bombRect = {
        gridOffset.x + tileSize * position.x,
        gridOffset.y + tileSize * position.y,
        tileSize,
        tileSize
    };

    switch (direction) {
        case Direction::up:
            bombRect.y -= static_cast<int>(offset);
            break;
            
        case Direction::down:
            bombRect.y += static_cast<int>(offset);
            break;
            
        case Direction::left:
            bombRect.x -= static_cast<int>(offset);
            break;
            
        case Direction::right:
            bombRect.x += static_cast<int>(offset);
            break;
            
        default:
            break;
    }

    return bombRect;
}
