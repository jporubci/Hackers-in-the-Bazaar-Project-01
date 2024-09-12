#include "player.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <cstdlib>

Player::Player(
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
    keyBuffer(),
    turnBuffer(),
    prevTime(),
    prevTimeValid(false)
{}

SDL_Point Player::get_next_position() const {
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

bool Player::check_collision() {
    SDL_Rect playerRect = {
        gridOffset.x + tileSize * position.x,
        gridOffset.y + tileSize * position.y,
        tileSize,
        tileSize
    };

    /* TODO: Check collision with enemies */

    return false;
}

void Player::set_direction(const Keyboard _keyboard, SDL_Keycode _key) {
    /* Is the key pressed down right now? */
    if (_keyboard.get_key(_key)) {
        /* Is the key not in the key buffer? */
        if (std::find(keyBuffer.begin(), keyBuffer.end(), _key) == keyBuffer.end()) {
            keyBuffer.push_front(_key);
            switch (_key) {
                case SDLK_w:
                    turnBuffer.push(Direction::up);
                    break;

                case SDLK_a:
                    turnBuffer.push(Direction::left);
                    break;

                case SDLK_s:
                    turnBuffer.push(Direction::down);
                    break;

                case SDLK_d:
                    turnBuffer.push(Direction::right);
                    break;

                default:
                    break;
            }
        }
    /* Is the key buffer not empty? */
    } else if (!keyBuffer.empty()) {
        /* Remove the key from the key buffer (if it exists) */
        for (auto it = keyBuffer.before_begin(); std::next(it) != keyBuffer.end(); ++it) {
            if (*std::next(it) == _key) {
                keyBuffer.erase_after(it);

                if (!keyBuffer.empty()) {
                    switch (keyBuffer.front()) {
                        case SDLK_w:
                            turnBuffer.push(Direction::up);
                            break;

                        case SDLK_a:
                            turnBuffer.push(Direction::left);
                            break;

                        case SDLK_s:
                            turnBuffer.push(Direction::down);
                            break;

                        case SDLK_d:
                            turnBuffer.push(Direction::right);
                            break;

                        default:
                            break;
                    }
                }
                break;
            }
        }
    }

    while (!turnBuffer.empty() && turnBuffer.front() == direction) {
        turnBuffer.pop();
    }
}

void Player::move() {
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

    /* Check if we are reversing directions */
    if (!turnBuffer.empty()) {
        switch (direction) {
            case Direction::up:
                if (turnBuffer.front() == Direction::down) {
                    --position.y;
                    offset = tileSize - offset;
                    direction = turnBuffer.front();
                    turnBuffer.pop();
                }
                break;
            case Direction::down:
                if (turnBuffer.front() == Direction::up) {
                    ++position.y;
                    offset = tileSize - offset;
                    direction = turnBuffer.front();
                    turnBuffer.pop();
                }
                break;
            case Direction::left:
                if (turnBuffer.front() == Direction::right) {
                    --position.x;
                    offset = tileSize - offset;
                    direction = turnBuffer.front();
                    turnBuffer.pop();
                }
                break;
            case Direction::right:
                if (turnBuffer.front() == Direction::left) {
                    ++position.x;
                    offset = tileSize - offset;
                    direction = turnBuffer.front();
                    turnBuffer.pop();
                }
                break;
            default:
                break;
        }
    }

    /* Move the player */
    offset += speed / (1e9 / _interval);
    prevTime = _currTime;

    switch (direction) {
        case Direction::up:
            position.y -= static_cast<int>(offset) / tileSize;
            if (position.y <= 0) {
                position.y = 0;
                offset = 0;
                if (!turnBuffer.empty()) {
                    direction = turnBuffer.front();
                    turnBuffer.pop();
                }
            }
            break;
        case Direction::down:
            position.y += static_cast<int>(offset) / tileSize;
            if (position.y >= numRows - 1) {
                position.y = numRows - 1;
                offset = 0;
                if (!turnBuffer.empty()) {
                    direction = turnBuffer.front();
                    turnBuffer.pop();
                }
            }
            break;
        case Direction::left:
            position.x -= static_cast<int>(offset) / tileSize;
            if (position.x <= 0) {
                position.x = 0;
                offset = 0;
                if (!turnBuffer.empty()) {
                    direction = turnBuffer.front();
                    turnBuffer.pop();
                }
            }
            break;
        case Direction::right:
            position.x += static_cast<int>(offset) / tileSize;
            if (position.x >= numCols - 1) {
                position.x = numCols - 1;
                offset = 0;
                if (!turnBuffer.empty()) {
                    direction = turnBuffer.front();
                    turnBuffer.pop();
                }
            }
            break;
        default:
            break;
    }

    if (offset >= tileSize) {
        if (!turnBuffer.empty()) {
            direction = turnBuffer.front();
            turnBuffer.pop();
        }
        offset = fmod(offset, tileSize);
    }
}

void Player::draw() {
    if (SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Rect playerRect = {
        gridOffset.x + tileSize * position.x,
        gridOffset.y + tileSize * position.y,
        tileSize,
        tileSize
    };

    switch (direction) {
        case Direction::up:
            playerRect.y -= static_cast<int>(offset);
            break;
            
        case Direction::down:
            playerRect.y += static_cast<int>(offset);
            break;
            
        case Direction::left:
            playerRect.x -= static_cast<int>(offset);
            break;
            
        case Direction::right:
            playerRect.x += static_cast<int>(offset);
            break;
            
        default:
            break;
    }

    SDL_RenderFillRect(renderer, &playerRect);
}

void Player::reset(SDL_Point _position, double _speed, Direction _direction) {
    position = _position;
    speed = _speed;
    offset = tileSize - 0.01;
    direction = Direction::right;
    keyBuffer.clear();
    while (!turnBuffer.empty()) {
        turnBuffer.pop();
    }
    prevTimeValid = false;
}
