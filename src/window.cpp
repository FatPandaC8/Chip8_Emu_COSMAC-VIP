#include "window.h"

bool Platform::input(uint8_t* keys) {
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            SDL_Keycode key = event.key.keysym.sym;

            if (key == SDLK_ESCAPE) {
                quit = true;
            }
            else if (key == SDLK_x) keys[0x0] = 1;
            else if (key == SDLK_1) keys[0x1] = 1;
            else if (key == SDLK_2) keys[0x2] = 1;
            else if (key == SDLK_3) keys[0x3] = 1;
            else if (key == SDLK_q) keys[0x4] = 1;
            else if (key == SDLK_w) keys[0x5] = 1;
            else if (key == SDLK_e) keys[0x6] = 1;
            else if (key == SDLK_a) keys[0x7] = 1;
            else if (key == SDLK_s) keys[0x8] = 1;
            else if (key == SDLK_d) keys[0x9] = 1;
            else if (key == SDLK_z) keys[0xA] = 1;
            else if (key == SDLK_c) keys[0xB] = 1;
            else if (key == SDLK_4) keys[0xC] = 1;
            else if (key == SDLK_r) keys[0xD] = 1;
            else if (key == SDLK_f) keys[0xE] = 1;
            else if (key == SDLK_v) keys[0xF] = 1;
        }
        else if (event.type == SDL_KEYUP) {
            SDL_Keycode key = event.key.keysym.sym;

            if (key == SDLK_x) keys[0x0] = 0;
            else if (key == SDLK_1) keys[0x1] = 0;
            else if (key == SDLK_2) keys[0x2] = 0;
            else if (key == SDLK_3) keys[0x3] = 0;
            else if (key == SDLK_q) keys[0x4] = 0;
            else if (key == SDLK_w) keys[0x5] = 0;
            else if (key == SDLK_e) keys[0x6] = 0;
            else if (key == SDLK_a) keys[0x7] = 0;
            else if (key == SDLK_s) keys[0x8] = 0;
            else if (key == SDLK_d) keys[0x9] = 0;
            else if (key == SDLK_z) keys[0xA] = 0;
            else if (key == SDLK_c) keys[0xB] = 0;
            else if (key == SDLK_4) keys[0xC] = 0;
            else if (key == SDLK_r) keys[0xD] = 0;
            else if (key == SDLK_f) keys[0xE] = 0;
            else if (key == SDLK_v) keys[0xF] = 0;
        }
    }

    return quit;
}
