#ifndef MYERBSSURF_H
#define MYERBSSURF_H

#include "knotvector.h"
#include "mysubsurface.h"
#include "animation.h"

#include <gmParametricsModule>

#include <parametrics/evaluators/gmerbsevaluator.h>

#include <memory>

enum LocalSurfaceType{SUBSURFACE, BEZIERSURFACE};

template <typename T>
class MyERBSSurf : public GMlib::PSurf<T, 3>
{
    GM_SCENEOBJECT(MyERBSSurf)

public:
    MyERBSSurf();
    MyERBSSurf(GMlib::PSurf<T,3> *original, int sampleU, int sampleV, int dim1, int dim2, LocalSurfaceType type);
    ~MyERBSSurf();

    void setAnimation(Animation *animation);

protected:
    GMlib::DMatrix<GMlib::PSurf<T,3>* > _c; // Control points

    void localSimulate(double dt);

    T getStartPU();
    T getStartPV();
    T getEndPU();
    T getEndPV();

    void eval(T u, T v, int d1, int d2, bool lu, bool lv);
    void makeKnotVector(KnotVector<T> &vector, int samples, int dim, bool closed, T start, T end);
    void makeBVector(GMlib::DVector<T> &bVector, const KnotVector<T> &k, int knotIndex, T t, int d);

    int findKnotIndex(T t, const KnotVector<T> &vector, bool closed);

    T mapKnot(T k, T start, T end);
    GMlib::DMatrix<GMlib::Vector<T,3> > makeCMatrix(T u, T v, int uIndex, int vIndex, int d1, int d2);


private:
    GMlib::PSurf<T,3>* _surface;
    GMlib::ERBSEvaluator<double> _evaluator;


    KnotVector<T> _u;
    KnotVector<T> _v;
    LocalSurfaceType _localSurfaceType;

    int _bezierDegree1;
    int _bezierDegree2;

    Animation* _animation;
    //bool _hasAnimation = false;


    void createSubSurfaces(GMlib::PSurf<T,3>* surf, int countU, int countV, bool closedU, bool closedV);

};

#include "myerbssurf.c"


#endif // MYERBSSURF_H
