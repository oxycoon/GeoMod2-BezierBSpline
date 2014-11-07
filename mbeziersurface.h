#ifndef MBEZIERSURFACE_H
#define MBEZIERSURFACE_H

#include <parametrics/gmpsurf>

template<typename T>
class MBezierSurface// : public GMlib::PSurf<T, 3>
{
public:
    //BezierBSpline(GMlib::PSurf<T,3> *c , T startU, T endU, T startV, T endV, T u, T v, int d);
    MBezierSurface();

    ~MBezierSurface(){}

    void evaluate(GMlib::DMatrix<T> &matrix, int d, T t, T delta);

protected:

    void init(){}
    void eval(GMlib::DMatrix<T> &matrix, int d, T t, T delta = 1);


private:
    int _d;
};

#include "mbeziersurface.c"

#endif // MBEZIERSURFACE_H
