#ifndef MBEZIERSURFACE_H
#define MBEZIERSURFACE_H

#include <parametrics/gmpsurf>

template<typename T>
class MBezierSurface : public GMlib::PSurf<T, 3>
{
    GM_SCENEOBJECT(MBezierSurface)
public:
    MBezierSurface();
    //MBezierSurface(GMlib::PSurf<T,3> *c , T startU, T endU, T startV, T endV, T u, T v, int d);
    MBezierSurface(GMlib::DMatrix<GMlib::Vector<T,3> > c);


    //~MBezierSurface(){}

    //void evaluate(GMlib::DMatrix<T> &matrix, int d, T t, T delta);

protected:
    void eval( T u, T v, int d1, int d2, bool lu = true, bool lv = true);

    T getStartPU();
    T getStartPV();
    T getEndPU();
    T getEndPV();


private:
    GMlib::DMatrix<GMlib::Vector<T,3> > _c;

    void computeBMatrix(GMlib::DMatrix<T> &matrix, int d, T t, T delta = 1);
};

#include "mbeziersurface.c"

#endif // MBEZIERSURFACE_H
