#include "grid.hpp"
#include <openssl/rand.h>
#include <openssl/err.h>
#include <cstring>
#include <climits>
#include <cstdlib>

Grid::Grid(
    SDL_Window* _window,
    SDL_Renderer* _renderer,
    int _numRows,
    int _numCols,
    int _tileSize
) :
    window(_window),
    renderer(_renderer),
    numRows(_numRows),
    numCols(_numCols),
    tileSize(_tileSize),
    gridSize(calc_grid_size()), sceneSize(calc_scene_size()), sceneOffset(calc_scene_offset()), gridOffset(calc_grid_offset()),
    grid(init_grid()),
    wallRects(calc_wall_rects()),
    fruitPos(init_fruit())
{}

SDL_Point Grid::calc_grid_size() const {
    return {numCols * tileSize, numRows * tileSize};
}

SDL_Point Grid::calc_scene_size() const {
    return {gridSize.x + tileSize * 2, gridSize.y + tileSize * 2};
}

SDL_Point Grid::calc_scene_offset() const {
    SDL_Point _windowSize;
    if (SDL_GetRendererOutputSize(renderer, &_windowSize.x, &_windowSize.y) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_GetRendererOutputSize() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    return {(_windowSize.x - sceneSize.x) / 2, (_windowSize.y - sceneSize.y) / 2};
}

SDL_Point Grid::calc_grid_offset() const {
    return {sceneOffset.x + tileSize, sceneOffset.y + tileSize};
}

std::vector<SDL_Rect> Grid::calc_wall_rects() const {
   return {
       {
           sceneOffset.x,
           sceneOffset.y,
           sceneSize.x,
           tileSize
       },
       {
           sceneOffset.x,
           gridOffset.y + gridSize.y,
           sceneSize.x,
           tileSize
       },
       {
           sceneOffset.x,
           gridOffset.y,
           tileSize,
           gridSize.y
       },
       {
           gridOffset.x + gridSize.x,
           gridOffset.y,
           tileSize,
           gridSize.y
       }
   };
}

std::vector<std::vector<Tile>> Grid::init_grid() {
    return {
        { Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall },
        { Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::wall, Tile::wall },
        { Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall, Tile::wall, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::player, Tile::empty, Tile::empty, Tile::empty, Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall, Tile::wall, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::wall, Tile::wall },
        { Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::wall, Tile::empty, Tile::wall, Tile::empty, Tile::wall },
        { Tile::wall, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::empty, Tile::wall },
        { Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall, Tile::wall }
    };
}

SDL_Point Grid::init_fruit() {
    /* New fruit tile */
    unsigned int _i;
    unsigned char _buf[sizeof(_i)];
    if (RAND_bytes(_buf, sizeof(_buf)) != 1) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "RAND_bytes() failed: %s", ERR_error_string(ERR_get_error(), nullptr));
        exit(EXIT_FAILURE);
    }

    std::memcpy(&_i, _buf, sizeof(_i));

    _i = static_cast<unsigned int>(static_cast<double>(_i) / UINT_MAX * (numRows * numCols - 2));

    /* Spawn a new fruit on a new empty tile */
    for (int y = 0; y < numRows; ++y) {
        for (int x = 0; x < numCols; ++x) {
            if (grid.at(y).at(x) == Tile::empty && _i-- == 0) {
                fruitPos = SDL_Point{x, y};
                grid.at(fruitPos.y).at(fruitPos.x) = Tile::fruit;
                return fruitPos;
            }
        }
    }

    return fruitPos;
}

SDL_Point Grid::get_grid_size() const {
    return gridSize;
}

SDL_Point Grid::get_scene_size() const {
    return sceneSize;
}

SDL_Point Grid::get_scene_offset() const {
    return sceneOffset;
}

SDL_Point Grid::get_grid_offset() const {
    return gridOffset;
}

/* TODO: Perhaps Grid::update should take a list of players
 * and enemies with their respective actions for a given
 * frame, and internally resolve the game logic. Currently,
 * Grid::update assumes the player is the only entity in the game. */
int Grid::update(SDL_Point _prevPos, SDL_Point _currPos) {
    auto& currTile = grid.at(_currPos.y).at(_currPos.x);
    auto& prevTile = grid.at(_prevPos.y).at(_prevPos.x);
    
    /* If position is same, do nothing */
    if (_currPos.x == _prevPos.x && _currPos.y == _prevPos.y) {
        return 0;
    }

    /* Wall collision */
    if (currTile == Tile::wall) {
        return 1;
    }

    /* Enemy collision */
    if (currTile == Tile::enemy) {
        /* TODO: Refine enemy collisions with player and enemy offsets */
        return -1;
    }

    /* Empty tile */
    if (currTile == Tile::empty) {
        currTile = Tile::player;
        prevTile = Tile::empty;
        return 0;
    }

    /* Fruit tile */
    if (currTile == Tile::fruit) {
        currTile = Tile::player;

        /* New fruit tile */
        unsigned int _i;
        unsigned char _buf[sizeof(_i)];
        if (RAND_bytes(_buf, sizeof(_buf)) != 1) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "RAND_bytes() failed: %s", ERR_error_string(ERR_get_error(), nullptr));
            exit(EXIT_FAILURE);
        }

        std::memcpy(&_i, _buf, sizeof(_i));

        _i = static_cast<unsigned int>(static_cast<double>(_i) / UINT_MAX * (256 - 154 - 2));

        /* Spawn a new fruit on a new empty tile */
        for (int y = 0; y < numRows; ++y) {
            for (int x = 0; x < numCols; ++x) {
                if (grid.at(y).at(x) == Tile::empty && _i-- == 0) {
                    fruitPos = SDL_Point{x, y};
                    grid.at(fruitPos.y).at(fruitPos.x) = Tile::fruit;
                    prevTile = Tile::empty;
                    return 0;
                }
            }
        }

        /* Unable to spawn a fruit location */
        return -1;
    }

    return 0;
}

void Grid::draw_grid() {
    if (SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i <= numRows; ++i) {
        SDL_RenderDrawLine(renderer,
            gridOffset.x,
            gridOffset.y + tileSize * i,
            gridOffset.x + gridSize.x,
            gridOffset.y + tileSize * i
        );
    }
    
    for (int i = 0; i <= numCols; ++i) {
        SDL_RenderDrawLine(renderer,
            gridOffset.x + tileSize * i,
            gridOffset.y,
            gridOffset.x + tileSize * i,
            gridOffset.y + gridSize.y
        );
    }

    for (int y = 0; y < numRows; ++y) {
        for (int x = 0; x < numCols; ++x) {
            draw_tile({x, y}, grid.at(y).at(x));
        }
    }
}

void Grid::draw_walls() {
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    for (const auto& wallRect : wallRects) {
        SDL_RenderFillRect(renderer, &wallRect);
    }
}

void Grid::draw_tile(const SDL_Point& tilePosition, const Tile& tileType) {
    int r, g, b;
    switch (tileType) {
        case Tile::fruit:
            r = 255; g =   0; b =   0;
            break;
        case Tile::wall:
            r =   0; g =   0; b = 255;
            break;
        default:
            return;
    }

    if (SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Rect _rect = {
        gridOffset.x + tilePosition.x * tileSize,
        gridOffset.y + tilePosition.y * tileSize,
        tileSize,
        tileSize
    };
    SDL_RenderFillRect(renderer, &_rect);
}

void Grid::reset() {
    grid = init_grid();
    fruitPos = init_fruit();
}
