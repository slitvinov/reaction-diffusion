#ifndef __REACT_DIFFUSE_H__
#define __REACT_DIFFUSE_H__

#include <stdlib.h>
#include <math.h>

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

void reaction_diffusion_system_init(reaction_diffusion_system *system,
                                    size_t width,
                                    size_t height,
                                    double f,
                                    double k,
                                    double du,
                                    double dv);

void reaction_diffusion_system_free(reaction_diffusion_system *system);

void reaction_diffusion_system_update(reaction_diffusion_system *system, double dt);

#endif

