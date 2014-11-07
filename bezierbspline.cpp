#include "bezierbspline.h"

template<typename T>
//BezierBSpline<T>::BezierBSpline(GMlib::PSurf<T,3> *c , T startU, T endU, T startV, T endV, T u, T v, int d)
BezierBSpline<T>::BezierBSpline()
{
    /*_d = d;
    _deltaU = 1 / (endU - startU);
    _deltaV = 1 / (endV - startV);*/
}

template<typename T>
inline
void BezierBSpline<T>::evaluate(GMlib::DMatrix<T> &matrix, int d, T t, T delta)
{
    eval(matrix, d, t, delta);
}

template<typename T>
inline
void BezierBSpline<T>::eval(GMlib::DMatrix<T> &m, int d, T t, T delta)
{
    int k = d + 1;

    //if the dimension is less or equal to 0, order is 1 and matrix has one element.
    if(d <= 0)
    {
        d = 1;
        m.setDim(1, 1);
        m[0][0] = 1;
        return;
    }
    else
    {
        m.setDim(k, k);
    }

    m[d-1][0] = 1-t;
    m[d-1][1] = t;

    for(int i = d -2; i >= 0; i++)
    {
       m[i][0] = (1-t) * m[i+1][0];

       for(int j = 1; j < d-i; j++)
       {
            m[i][j] = t * m[i+1][j-1] + (1-t) * m[i+1][j];
       }
       m[i][d-1] = t * m[i+1][d-i-1];
    }

    T verifier;
    for(int i = 0; i <= k; i++)
    {
        for(int j = 0; j <= k; j++)
        {
            verifier += m[i][j];
        }
        std::cout << "Verifying line #" << i << " before derivatives: " << verifier << std::endl;
        verifier = 0;
    }


}

template<typename T>
inline
T BezierBSpline<T>::getEndPU()
{
    return (T) 1.0;
}

template<typename T>
inline
T BezierBSpline<T>::getEndPV()
{
    return (T) 1.0;
}

template<typename T>
inline
T BezierBSpline<T>::getStartPU()
{
    return (T) 0.0;
}

template<typename T>
inline
T BezierBSpline<T>::getStartPV()
{
    return (T) 0.0;
}
