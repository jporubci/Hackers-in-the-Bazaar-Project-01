#include "game.hpp"
#include <SDL.h>
#include <vector>

Game::Game(
    SDL_Window* _window,
    SDL_Renderer* _renderer
) :
    window(_window),
    renderer(_renderer),
    state(State::newGame),
    keyboard(),
    numRows(16),
    numCols(16),
    tileSize(48),
    grid(
        window,
        renderer,
        numRows,
        numCols,
        tileSize
    ),
    player(
        window,
        renderer,
        numRows,
        numCols,
        tileSize,
        grid.get_grid_offset(),
        SDL_Point({2, 8}),
        5.0 * tileSize,
        Direction::right
    ),
    enemies(
        {
            Enemy(
                window,
                renderer,
                numRows,
                numCols,
                tileSize,
                grid.get_grid_offset(),
                SDL_Point({7, 7}),
                3.0 * tileSize,
                Direction::right
            ),
            Enemy(
                window,
                renderer,
                numRows,
                numCols,
                tileSize,
                grid.get_grid_offset(),
                SDL_Point({8, 7}),
                3.0 * tileSize,
                Direction::right
            ),
            Enemy(
                window,
                renderer,
                numRows,
                numCols,
                tileSize,
                grid.get_grid_offset(),
                SDL_Point({7, 8}),
                3.0 * tileSize,
                Direction::right
            ),
            Enemy(
                window,
                renderer,
                numRows,
                numCols,
                tileSize,
                grid.get_grid_offset(),
                SDL_Point({8, 8}),
                3.0 * tileSize,
                Direction::right
            )
        }
    ),
    hud(
        window,
        renderer
    )
{}

bool Game::gameOn() const {
    return state != State::quitGame;
}

void Game::poll() {
    for (SDL_Event _event; SDL_PollEvent(&_event) != 0;) {
        if (_event.type == SDL_QUIT || (_event.type == SDL_KEYDOWN && _event.key.keysym.sym == SDLK_ESCAPE)) {
            state = State::quitGame;
            return;
        }
        
        if (_event.type == SDL_KEYDOWN || _event.type == SDL_KEYUP) {
            if (_event.key.keysym.sym == SDLK_w || _event.key.keysym.sym == SDLK_a || _event.key.keysym.sym == SDLK_s || _event.key.keysym.sym == SDLK_d) {
                keyboard.set_key(&_event);
                player.set_direction(keyboard, _event.key.keysym.sym);
            } else if (_event.key.keysym.sym == SDLK_SPACE) {
                player.bombs.emplace_back(Bomb(
                    window,
                    renderer,
                    numRows,
                    numCols,
                    tileSize,
                    grid.get_grid_offset(),
                    player.get_next_position(),
                    7.0 * tileSize,
                    player.direction
                ));
            }
        }
    }
}

void Game::step() {
    std::vector<SDL_Keycode> _movementKeys{SDLK_w, SDLK_a, SDLK_s, SDLK_d};
    SDL_Point _pos, _prevPos, _currPos;
    int _turned, _status;
    SDL_Rect playerRect;
    std::vector<int> bombIndexes;
    SDL_Rect explosion, enemyRect;
    size_t i;
    switch (state) {
        case State::newGame:
            for (const auto _key : _movementKeys) {
                if (keyboard.get_key(_key)) {
                    state = State::playGame;
                    break;
                }
            }
            break;
            
        case State::playGame:
            _pos = player.get_position();
            _prevPos = player.get_next_position();
            _turned = player.move();
            playerRect = player.get_rect();
            for (auto& enemy : enemies) {
                enemy.move(grid.grid);
                enemy.check_collision(playerRect);
                if (enemy.check_collision(playerRect)) {
                    state = State::gameOver;
                    return;
                }
            }
            _currPos = player.get_next_position();
            _status = grid.update(_prevPos, _currPos);
            for (auto& bomb : player.bombs) {
                bombIndexes.push_back(0);
                if (!bomb.exploding) {
                    bomb.move();
                }
                if (bomb.check_collision(grid.grid)) {
                    explosion = bomb.explode();
                    if (explosion.h == 0) {
                        bombIndexes.back() = 1;
                    } else {
                        for (i = 0; i < enemies.size();) {
                            enemyRect = enemies.at(i).get_rect();
                            if (SDL_HasIntersection(&explosion, &enemyRect) == SDL_TRUE) {
                                enemies.erase(enemies.begin() + i);
                            } else {
                                ++i;
                            }
                        }
                    }
                }
            }
            for (i = 0; i < player.bombs.size();) {
                if (player.bombs.at(i).exploding && player.bombs.at(i).lifetime > 0) {
                    if (SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE) < 0) {
                        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
                        exit(EXIT_FAILURE);
                    }

                    explosion = player.bombs.at(i).get_rect();
                    explosion.x -= tileSize;
                    explosion.y -= tileSize;
                    explosion.h *= 3;
                    explosion.w = explosion.h;
                    SDL_RenderFillRect(renderer, &explosion);
                }
                if (bombIndexes.at(i) && player.bombs.at(i).lifetime <= 0) {
                    player.bombs.erase(player.bombs.begin() + i);
                    bombIndexes.erase(bombIndexes.begin() + i);
                } else {
                    ++i;
                }
            }
            if (_status < 0) {
                state = State::gameOver;
            } else if (_status == 1) {
                if (_turned) {
                    _prevPos.x -= _currPos.x - _prevPos.x;
                    _prevPos.y -= _currPos.y - _prevPos.y;
                    _pos = _prevPos;
                }
                player.collided_with_wall(_turned, _pos);
            } else if (enemies.size() == 0) {
                state = State::gameOver;
            }
            break;
            
        case State::gameOver:
            SDL_Delay(2000);
            for (SDL_Event _event; SDL_PollEvent(&_event) != 0;);
            keyboard.reset();
            player.reset(SDL_Point({2, 8}), 5.0 * tileSize, Direction::right);
            if (enemies.size() == 0) {
                hud.increment_level();
            }
            enemies.clear();
            enemies.emplace_back(Enemy(
                window,
                renderer,
                numRows,
                numCols,
                tileSize,
                grid.get_grid_offset(),
                SDL_Point({7, 7}),
                3.0 * tileSize,
                Direction::right
            ));
            enemies.emplace_back(Enemy(
                window,
                renderer,
                numRows,
                numCols,
                tileSize,
                grid.get_grid_offset(),
                SDL_Point({7, 8}),
                3.0 * tileSize,
                Direction::right
            ));
            enemies.emplace_back(Enemy(
                window,
                renderer,
                numRows,
                numCols,
                tileSize,
                grid.get_grid_offset(),
                SDL_Point({8, 7}),
                3.0 * tileSize,
                Direction::right
            ));
            enemies.emplace_back(Enemy(
                window,
                renderer,
                numRows,
                numCols,
                tileSize,
                grid.get_grid_offset(),
                SDL_Point({8, 8}),
                3.0 * tileSize,
                Direction::right
            ));
            grid.reset();
            state = State::newGame;
            break;
            
        case State::quitGame:
            break;
            
        default:
            break;
    }
}

void Game::draw() {
    grid.draw_grid();
    for (auto& enemy : enemies) {
        enemy.draw();
    }
    player.draw();
    hud.draw();
}
