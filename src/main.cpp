#include <SDL.h>
#include <iostream>
#include "reactdiffuse.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

const int MATRIX_W = 200;
const int MATRIX_H = 200;

const float RATIO_X = WINDOW_WIDTH / MATRIX_W;
const float RATIO_Y = WINDOW_HEIGHT / MATRIX_H;

// default
//ReactDiffuse reactDiffuse(MATRIX_W, MATRIX_H, .055, .062);

// mitosis
//ReactDiffuse reactDiffuse(MATRIX_W, MATRIX_H, .0367, .0649);

// coral
ReactDiffuse reactDiffuse(MATRIX_W, MATRIX_H, .0545, .062);

Uint32 SDL_MapHSV (SDL_PixelFormat *fmt, float h, float s, float v) {
    float r, g, b;
    int hi = (int)(h/60.f) %6;
    float f = h/60.f - (int)h/60;
    float p = v * (1.f - s);
    float q = v * (1.f - f*s);
    float t = v * (1.f - (1.f - f) * s);

    switch (hi) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return SDL_MapRGB(fmt, r*255, g*255, b*255);
}

void draw(SDL_Surface *surface) {
    SDL_Rect rect;
    rect.w = RATIO_X;
    rect.h = RATIO_Y;

    for (int x = 0; x < MATRIX_W; x++) {
        for (int y = 0; y < MATRIX_H; y++) {
            double u = reactDiffuse.get(reactDiffuse.U, x, y);
            double v = reactDiffuse.get(reactDiffuse.V, x, y);

            rect.x = RATIO_X * x;
            rect.y = RATIO_Y * y;

            SDL_FillRect(surface,
                         &rect,
                         SDL_MapHSV( surface->format, v * 360, v, u ));
                         //SDL_MapRGB( surface->format, u * 255, 0, v * 255 ));
        }
    }
}

int main(int argc, char **argv) {
    /* SDL Setup */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        std::cerr <<  "Could not initialize SDL " << SDL_GetError() << std::endl;
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_Surface *surface;

    surface = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE);
    if ( surface == NULL ) {
        std::cerr <<  "Could not setup screen to resolution "
                  << WINDOW_HEIGHT << 'x' <<  WINDOW_HEIGHT
                  << ' ' <<  SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_Event event;
    bool leftPressed = false;
    bool rightPressed = false;

    while(true) {
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        leftPressed = event.button.state == SDL_PRESSED;
                        break;
                    case SDL_BUTTON_RIGHT:
                        rightPressed = event.button.state == SDL_PRESSED;
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (leftPressed) {
                    reactDiffuse.set(reactDiffuse.V,
                                     (int)(event.motion.x / RATIO_X),
                                     (int)(event.motion.y / RATIO_Y),
                                     1);
                }
        }

        draw(surface);
        SDL_Flip(surface);

        reactDiffuse.update(1.);
    }

    return 0;
}

