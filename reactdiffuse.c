#include "reactdiffuse.h"

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

