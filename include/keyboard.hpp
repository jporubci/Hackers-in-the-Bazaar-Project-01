#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <SDL.h>
#include <unordered_map>

class Keyboard {
public:
    Keyboard();
    
    bool get_key(SDL_Keycode _key) const;
    void set_key(SDL_Event* _event);
    void reset();
    
private:
    std::unordered_map<SDL_Keycode, bool> keymap;
};

#endif
