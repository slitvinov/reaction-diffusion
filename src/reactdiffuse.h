#ifndef __REACT_DIFFUSE_H__
#define __REACT_DIFFUSE_H__

#include <cstdlib>

const double DU = 1.0;
const double DV = 0.5;

class ReactDiffuse {

    public:

        ReactDiffuse(int width, int height, double f, double k);
        ~ReactDiffuse();

        void update(double dt);

        double get(double *m, int x, int y);
        void set(double *m, int x, int y, double v);

        int width;
        int height;
        double *U;
        double *V;

    private:
        double *swapU;
        double *swapV;

        double f;
        double k;

        void calculateLaplacian(double *from, double *to);
};

#endif
