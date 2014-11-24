#ifndef MYBSPLINE_H
#define MYBSPLINE_H

#include <parametrics/gmpsurf>

class MyBSpline
{
public:
    MyBSpline();

    ~MyBSpline(){}

    void evaluate(GMlib::DMatrix<T> &matrix, int d, T t, T delta);

protected:

    void init(){}
    void eval(GMlib::DMatrix<T> &matrix, int d, T t, T delta = 1);


private:
    int _d;
};


#include "mybspline.c"
#endif // MYBSPLINE_H
