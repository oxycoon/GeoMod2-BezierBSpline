#ifndef MYERBSSURF_H
#define MYERBSSURF_H

#include <gmParametricsModule>
#include "knotvector.h"

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

    KnotVector _u;
    KnotVector _v;

    int _bezierDegree1;
    int _bezierDegree2;

    T getStartPU();
    T getStartPV();
    T getEndPU();
    T getEndPV();

    void localSimulate(double dt);
    void eval(T u, T v, int d1, int d2, bool lu, bool lv);

    KnotVector makeKnotVector(KnotVector &vector, int sample, int dim, bool closed, T start, T end);
    int findKnotIndex(T t, const KnotVector &theVector, bool closed);
    float mapKnot(T u, T start, T end);

private:


};

#include "myerbssurf.c"


#endif // MYERBSSURF_H
