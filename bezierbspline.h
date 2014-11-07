#ifndef BEZIERBSPLINE_H
#define BEZIERBSPLINE_H

#include <parametrics/gmpsurf>

template<typename T>
class BezierBSpline : public GMlib::PSurf<T, 3>
{
public:
    //BezierBSpline(GMlib::PSurf<T,3> *c , T startU, T endU, T startV, T endV, T u, T v, int d);
    BezierBSpline();

    virtual ~BezierBSpline();

    void evaluate(GMlib::DMatrix<T> &matrix, int d, T t, T delta);

protected:

    virtual void init();
    void eval(GMlib::DMatrix<T> &matrix, int d, T t, T delta = 1);

    T getEndPU();
    T getEndPV();
    T getStartPU();
    T getStartPV();

private:
    int _d;
    T _deltaU, _deltaV;
};

#endif // BEZIERBSPLINE_H
