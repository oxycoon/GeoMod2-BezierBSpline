#ifndef MYBSPLINE_H
#define MYBSPLINE_H

#include <parametrics/gmpsurf>

template<typename T>
class MyBSpline : public GMlib::PSurf<T,3>
{
public:
    MyBSpline();

    ~MyBSpline(){}

    void evaluate(GMlib::DMatrix<T> &matrix, int d, T t, T delta);

protected:

    void init(){}
    virtual void eval(GMlib::DMatrix<T> &matrix, int d, T t, T delta = 1);


private:
    GMlib::DMatrix<GMlib::PSurf<T,3>*> _c;

    GMlib::DVector<T> _u;
    GMlib::DVector<T> _v;

    bool isClosedU;
    bool isClosedV;
};


#include "mybspline.c"
#endif // MYBSPLINE_H
