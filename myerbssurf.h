#ifndef MYERBSSURF_H
#define MYERBSSURF_H

#include <gmParametricsModule>

#include <parametrics/evaluators/gmerbsevaluator.h>

#include "knotvector.h"
#include "mysubsurface.h"

enum LocalSurfaceType{SUBSURFACE, BEZIERSURFACE};

template <typename T>
class MyERBSSurf : public GMlib::PSurf<T, 3>
{
public:
    MyERBSSurf();
    MyERBSSurf(GMlib::PSurf<T,3> *original, int sampleU, int sampleV, int dim1, int dim2);
    ~MyERBSSurf();

protected:


    T getStartPU();
    T getStartPV();
    T getEndPU();
    T getEndPV();

    void localSimulate(double dt);
    void eval(T u, T v, int d1, int d2, bool lu, bool lv);
    void makeKnotVector(KnotVector<T> &vector, int samples, int dim, bool closed, T start, T end);
    void makeBVector(GMlib::DVector<T> &bVector, const KnotVector<T> &k, int knotIndex, T t, int d);

    int findKnotIndex(T t, const KnotVector<T> &vector, bool closed);

    T mapKnot(T k, T start, T end);
    GMlib::DMatrix<GMlib::Vector<T,3> > makeCMatrix(T u, T v, int uIndex, int vIndex, int d1, int d2);


private:
    GMlib::PSurf<T,3>* _surface;
    GMlib::ERBSEvaluator<double> _evaluator;
    GMlib::DMatrix<GMlib::PSurf<T,3>* > _c;

    KnotVector<T> _u;
    KnotVector<T> _v;
    LocalSurfaceType _localSurfaceType;

    int _bezierDegree1;
    int _bezierDegree2;



    void createSubSurfaces(GMlib::PSurf<T,3>* surf, int countU, int countV, bool closedU, bool closedV);

};

#include "myerbssurf.c"


#endif // MYERBSSURF_H
