#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define DU 1.0
#define DV 0.5

typedef struct {
    size_t width;
    size_t height;
    double f;
    double k;
    double du;
    double dv;

    double *U;
    double *V;
    double *swapU;
    double *swapV;
} reaction_diffusion_system ;

#define reaction_diffusion_system_get(s, m, x, y) (m[(((y) + (s)->height) % (s)->height)*(s)->width + (((x) + (s)->width)  % (s)->width)])
#define reaction_diffusion_system_set(s, m, x, y, v) (m[(((y) + (s)->height) % (s)->height)*(s)->width + (((x) + (s)->width)  % (s)->width)] = fmin(1, fmax(-1, (v))))

void reaction_diffusion_system_init(reaction_diffusion_system *s,
                                    size_t width,
                                    size_t height,
                                    double f,
                                    double k,
                                    double du,
                                    double dv) {

    s->width = width;
    s->height = height;
    s->f = f;
    s->k = k;
    s->du = du;
    s->dv = dv;
    s->U = (double*)malloc(width * height * sizeof(double));
    s->V = (double*)malloc(width * height * sizeof(double));
    s->swapU = (double*)malloc(width * height * sizeof(double));
    s->swapV = (double*)malloc(width * height * sizeof(double));

    // initialize
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            reaction_diffusion_system_set(s, s->U, x, y, 1);
            reaction_diffusion_system_set(s, s->V, x, y, 0);
        }
    }
}

void reaction_diffusion_system_free(reaction_diffusion_system *s) {
    free(s->U);
    free(s->V);
    free(s->swapU);
    free(s->swapV);
}

double get_laplacian(reaction_diffusion_system *s, double *from, int x, int y) {
    return .05 * reaction_diffusion_system_get(s, from, x-1, y-1) +
           .2  * reaction_diffusion_system_get(s, from, x-1, y  ) +
           .05 * reaction_diffusion_system_get(s, from, x-1, y+1) +
           .2  * reaction_diffusion_system_get(s, from, x  , y-1) +
           -1. * reaction_diffusion_system_get(s, from, x  , y  ) +
           .2  * reaction_diffusion_system_get(s, from, x  , y+1) +
           .05 * reaction_diffusion_system_get(s, from, x+1, y-1) +
           .2  * reaction_diffusion_system_get(s, from, x+1, y  ) +
           .05 * reaction_diffusion_system_get(s, from, x+1, y+1);
}

void reaction_diffusion_system_update(reaction_diffusion_system *s, double dt) {
    double *temp;

    // calculate new concentrations, and write the new value to swaps
    for (int x = 0; x < s->width; x++) {
        for (int y = 0; y < s->height; y++) {
            double u = reaction_diffusion_system_get(s, s->U, x, y);
            double v = reaction_diffusion_system_get(s, s->V, x, y);
            double deltaU = s->du*get_laplacian(s, s->U, x, y)
                            - (u * v * v)
                            + s->f*(1. - u);
            double deltaV = s->dv*get_laplacian(s, s->V, x, y)
                            + (u * v * v)
                            - (s->k + s->f)*v;

            reaction_diffusion_system_set(s, s->swapU, x, y, u + deltaU*dt);
            reaction_diffusion_system_set(s, s->swapV, x, y, v + deltaV*dt);
        }
    }

    // swap
    temp = s->U;
    s->U = s->swapU;
    s->swapU = temp;

    temp = s->V;
    s->V = s->swapV;
    s->swapV = temp;
}

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define MATRIX_W 200
#define MATRIX_H 200

#define RATIO_X (WINDOW_WIDTH / MATRIX_W)
#define RATIO_Y (WINDOW_HEIGHT / MATRIX_H)

reaction_diffusion_system rds;

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
            double u = reaction_diffusion_system_get(&rds, rds.U, x, y);
            double v = reaction_diffusion_system_get(&rds, rds.V, x, y);

            rect.x = RATIO_X * x;
            rect.y = RATIO_Y * y;

            SDL_FillRect(surface,
                         &rect,
                         SDL_MapHSV(surface->format, 0, 0, .5 + .5*sin(20*v + 10*u) ));
                         //SDL_MapHSV(surface->format, (int)(u * 1000) % 360, v, .9*v + .1*u));
        }
    }
}

int main(int argc, char **argv) {
    /* SDL SEtup */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_Surface *surface;

    surface = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE);
    if ( surface == NULL ) {
        fprintf(stderr, "Could not setup screen to resolution %dx%d : %s\n", 
                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_GetError());
        exit(1);
    }

    // default
    reaction_diffusion_system_init(&rds, MATRIX_W, MATRIX_H, .055, .062, 1.0, 0.5);

    // mitosis
    // reaction_diffusion_system_init(&rds, MATRIX_W, MATRIX_H, .0367, .0649, 1.0, 0.5);

    // coral
    // reaction_diffusion_system_init(&rds, MATRIX_W, MATRIX_H, .0545, .062, 1.0, 0.5);


    SDL_Event event;
    int leftPressed = 0;
    int rightPressed = 0;
    int mustExit = 0;

    while (!mustExit) {

        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                mustExit = 1;
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
                    reaction_diffusion_system_set(&rds, rds.V,
                                                  (int)(event.motion.x / RATIO_X),
                                                  (int)(event.motion.y / RATIO_Y),
                                                  1);
                }
        }

        draw(surface);
        SDL_Flip(surface);

        reaction_diffusion_system_update(&rds, 1.);
    }

    reaction_diffusion_system_free(&rds);

    return 0;
}

