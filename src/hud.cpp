#include "hud.hpp"
#include <cmath>
#include <string>
#include <cstdlib>

Hud::Hud(
    SDL_Window* _window,
    SDL_Renderer* _renderer
) :
    window(_window),
    renderer(_renderer),
    textColor({255, 255, 255, SDL_ALPHA_OPAQUE}),
    font(init_font("/System/Library/Fonts/Monaco.ttf", 14)),
    textureRect(set_textureRect("Level: "))
{}

TTF_Font* Hud::init_font(const char* _fontPath, int _fontPtSize) {
    if (TTF_Init() == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_Init() failed: %s", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    
    TTF_Font* _font = TTF_OpenFont(_fontPath, _fontPtSize);
    if (_font == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont() failed: %s", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    
    return _font;
}

Hud::TextureRect Hud::set_textureRect(const char* _fpsText) {
    SDL_Surface* _surface = TTF_RenderUTF8_Solid(font, _fpsText, textColor);
    if (_surface == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_RenderUTF8_Solid() failed: %s", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    
    SDL_Texture* _texture = SDL_CreateTextureFromSurface(renderer, _surface);
    if (_texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    int _windowWidth;
    if (SDL_GetRendererOutputSize(renderer, &_windowWidth, nullptr) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_GetRendererOutputSize() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    SDL_Rect _rect = SDL_Rect{0, 0, _surface->w, _surface->h};
    SDL_FreeSurface(_surface);
    
    return TextureRect{_texture, _rect};
}

void Hud::draw() {
    // track number of lives, points

    ++frameCount;
    
    if (SDL_RenderCopy(renderer, textureRect.texture, nullptr, &textureRect.rect) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_RenderCopy() failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    SDL_DestroyTexture(textureRect.texture);
    textureRect = set_textureRect("Level: ");
    
    frameCount = 0;
}

void Hud::shutdown() {
    if (font != nullptr) {
        TTF_CloseFont(font);
    }
    
    if (textureRect.texture != nullptr) {
        SDL_DestroyTexture(textureRect.texture);
    }
}
