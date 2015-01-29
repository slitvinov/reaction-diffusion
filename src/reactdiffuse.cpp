#include "reactdiffuse.h"
#include <math.h>
#include <iostream>

ReactDiffuse::ReactDiffuse(int width, int height, double f, double k) :
    width(width), height(height), f(f), k(k) {
    U = new double[width * height];
    V = new double[width * height];

    // initialize
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            set(U, x, y, 1);
            set(V, x, y, 0);
        }
    }

    swapU = new double[width * height];
    swapV = new double[width * height];
}

ReactDiffuse::~ReactDiffuse() {
    delete U;
    delete V;
    delete swapU;
    delete swapV;
}

void ReactDiffuse::update(double dt) {
    // write laplacians calculation to swaps
    double *temp;
    calculateLaplacian(U, swapU);
    calculateLaplacian(V, swapV);

    // calculate new concentrations, and write the new value to swaps
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double u = get(U, x, y);
            double v = get(V, x, y);
            double deltaU = DU*get(swapU, x, y) - (u * v * v) + f*(1. - u);
            double deltaV = DV*get(swapV, x, y) + (u * v * v) - (k + f)*v;

            set(swapU, x, y, u + deltaU*dt);
            set(swapV, x, y, v + deltaV*dt);
        }
    }

    // swap
    temp = U;
    U = swapU;
    swapU = temp;

    temp = V;
    V = swapV;
    swapV = temp;
}

double ReactDiffuse::get(double *m, int x, int y) {
    x = (x + width) % width;
    y = (y + height) % height;
    return m[y*width + x];
}

void ReactDiffuse::set(double *m, int x, int y, double v) {
    x = (x + width) % width;
    y = (y + height) % height;
    m[y*width + x] = v; // fmin(1, fmax(0, v));
}

void ReactDiffuse::calculateLaplacian(double *from, double *to) {
    double v;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            v = .05 * get(from, x-1, y-1) +
                .2  * get(from, x-1, y  ) +
                .05 * get(from, x-1, y+1) +
                .2  * get(from, x  , y-1) +
                -1. * get(from, x  , y  ) +
                .2  * get(from, x  , y+1) +
                .05 * get(from, x+1, y-1) +
                .2  * get(from, x+1, y  ) +
                .05 * get(from, x+1, y+1);
            set(to, x, y, v);
        }
    }
}

