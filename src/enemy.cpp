#include "enemy.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <cstdlib>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "tile.hpp"

Enemy::Enemy(
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
    offset(tileSize - 0.01),
    direction(_direction),
    prevTime(),
    prevTimeValid(false)
{}

SDL_Point Enemy::get_position() const {
    return position;
}

SDL_Point Enemy::get_next_position() const {
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

bool Enemy::check_collision(const SDL_Rect& playerRect) {
    SDL_Rect enemyRect = get_rect();

    return SDL_HasIntersection(&playerRect, &enemyRect) == SDL_TRUE;
}

void Enemy::set_direction() {
    /* New direction */
    unsigned int _i;
    unsigned char _buf[sizeof(_i)];
    if (RAND_bytes(_buf, sizeof(_buf)) != 1) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "RAND_bytes() failed: %s", ERR_error_string(ERR_get_error(), nullptr));
        exit(EXIT_FAILURE);
    }

    std::memcpy(&_i, _buf, sizeof(_i));

    _i = static_cast<unsigned int>(static_cast<double>(_i) / UINT_MAX * 16);

    switch (_i) {
        case 0:
            direction = Direction::up;
            break;
        case 1:
            direction = Direction::down;
            break;
        case 2:
            direction = Direction::left;
            break;
        case 3:
            direction = Direction::right;
            break;
        default:
            break;
    }
}

void Enemy::collided_with_wall(const bool turned, const SDL_Point prevPos) {
    position = prevPos;
    offset = tileSize - 0.0001;
}

void Enemy::move(const std::vector<std::vector<Tile>>& grid) {
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

    /* Move the enemy */
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
        offset = 0;
        set_direction();
        SDL_Point nextPos = get_next_position();
        while (grid.at(nextPos.y).at(nextPos.x) == Tile::wall) {
            set_direction();
            nextPos = get_next_position();
        }
    }
}

void Enemy::draw() {
    if (SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Rect enemyRect = get_rect();

    SDL_RenderFillRect(renderer, &enemyRect);
}

void Enemy::reset(SDL_Point _position, double _speed, Direction _direction) {
    position = _position;
    speed = _speed;
    offset = tileSize - 0.01;
    direction = Direction::right;
    prevTimeValid = false;
}

SDL_Rect Enemy::get_rect() const {
    SDL_Rect enemyRect = {
        gridOffset.x + tileSize * position.x,
        gridOffset.y + tileSize * position.y,
        tileSize,
        tileSize
    };

    switch (direction) {
        case Direction::up:
            enemyRect.y -= static_cast<int>(offset);
            break;
            
        case Direction::down:
            enemyRect.y += static_cast<int>(offset);
            break;
            
        case Direction::left:
            enemyRect.x -= static_cast<int>(offset);
            break;
            
        case Direction::right:
            enemyRect.x += static_cast<int>(offset);
            break;
            
        default:
            break;
    }

    return enemyRect;
}