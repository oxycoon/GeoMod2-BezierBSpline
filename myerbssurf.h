#ifndef MYERBSSURF_H
#define MYERBSSURF_H

#include <gmParametricsModule>
#include "knotvector.h"

enum LocalSurfaceType{SUBSURFACE, BEZIERSURFACE};

template <typename T>
class MyERBSSurf : public GMlib::PSurf<T, 3>
{
public:
    MyERBSSurf();
    MyERBSSurf(GMlib::PSurf<T,3> &original);
    ~MyERBSSurf(){}

protected:
    GMlib::PSurf<T,3>* _surface;
    GMlib::ERBSEvaluator<T> _evaluator;

    KnotVector<T> _u;
    KnotVector<T> _v;
    LocalSurfaceType _localSurfaceType;

    int _bezierDegree1;
    int _bezierDegree2;

    T getStartPU();
    T getStartPV();
    T getEndPU();
    T getEndPV();

    void localSimulate(double dt);
    void eval(T u, T v, int d1, int d2, bool lu, bool lv);
    void makeKnotVector(KnotVector<T> &vector, int samples, int dim, bool closed, T start, T end);

    int findKnotIndex(T t, const KnotVector<T> &theVector, bool closed);

    T mapKnot(T k, T start, T end);


private:
    void createSubSurfaces(GMlib::PSurf<T,3>* surf, int countU, int countV, bool closedU, bool closedV);

};

#include "myerbssurf.c"


#endif // MYERBSSURF_H
