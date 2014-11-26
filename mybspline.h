#ifndef MYBSPLINE_H
#define MYBSPLINE_H

#include <parametrics/gmpsurf>

#include "knotvector.h"

template<typename T>
class MyBSpline : public GMlib::PSurf<T,3>
{
public:
    MyBSpline();

    ~MyBSpline(){}



protected:

    void init(){}
    void eval(T u, T v, int d1, int d2, bool lu, bool lv);


private:
    GMlib::DMatrix<GMlib::PSurf<T,3>*> _c;

    GMlib::DVector<T> _u;
    GMlib::DVector<T> _v;

    bool isClosedU;
    bool isClosedV;

    void computeBMatrix(GMlib::DMatrix<T> &matrix, int d, T t, T delta);

    int findKnotIndex(T t, const KnotVector<T> &vector, bool closed);
};


#include "mybspline.c"
#endif // MYBSPLINE_H
