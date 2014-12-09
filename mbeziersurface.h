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
    MBezierSurface(GMlib::PSurf<T,3> *surf, T startU, T endU, T startV, T endV, T u, T v, int dim1, int dim2, bool closedU = false, bool closedV = false);


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
    GMlib::PSurf<T,3>* _originalSurface;

    bool _isClosedU;
    bool _isClosedV;

    T _startU;
    T _startV;
    T _endU;
    T _endV;
    T _deltaU;
    T _deltaV;
    T _u;
    T _v;

    int _dim1;
    int _dim2;

    void computeBMatrix(GMlib::DMatrix<T> &matrix, int d, T t, T delta = 1);
    void computeControlPoints(GMlib::PSurf<T,3> *surf, T u, T v, T uScaled, T vScaled);
};

#include "mbeziersurface.c"

#endif // MBEZIERSURFACE_H
