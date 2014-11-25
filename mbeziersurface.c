#include "mbeziersurface.h"

template<typename T>
inline
//MBezierSurface<T>::MBezierSurface(GMlib::PSurf<T,3> *c , T startU, T endU, T startV, T endV, T u, T v, int d)
MBezierSurface<T>::MBezierSurface()
{
    /*_d = d;
    _deltaU = 1 / (endU - startU);
    _deltaV = 1 / (endV - startV);*/
}

template<typename T>
inline
MBezierSurface<T>::MBezierSurface(GMlib::DMatrix<GMlib::Vector<T,3> > c)
{
    _c = c;
}

template<typename T>
void MBezierSurface<T>::eval(T u, T v, int d1, int d2, bool, bool)
{
    this->_p.setDim(d1 + 1, d2 + 1);

    GMlib::DMatrix<T> b1;
    GMlib::DMatrix<T> b2;

    computeBMatrix(b1, d1, u);
    computeBMatrix(b2, d2, v);

    this->_p = (b1 * _c) ^ b2.transpose();
}

template<typename T>
T MBezierSurface<T>::getStartPU()
{
    return 0;
}

template<typename T>
T MBezierSurface<T>::getStartPV()
{
    return 0;
}

template<typename T>
T MBezierSurface<T>::getEndPU()
{
    return 1;
}

template<typename T>
T MBezierSurface<T>::getEndPV()
{
    return 1;
}

/**
 * @brief MBezierSurface<T>::computeBMatrix
 * @param m
 * @param d
 * @param t
 * @param delta
 */
template<typename T>
void MBezierSurface<T>::computeBMatrix(GMlib::DMatrix<T> &m, int d, T t, T delta)
{
    int dim = d + 1;

    //if the dimension is less or equal to 0, order is 1 and matrix has one element.
    if(d <= 0)
    {
        d = 1;
        m.setDim(1, 1);
        m[0][0] = 1;
        return;
    }
    //Other wise set the dimension
    else
    {
        m.setDim(dim, dim);
    }

    //Sets the first values in the matrix.
    m[d-1][0] = 1-t;
    m[d-1][1] = t;

    //Calculates the top half of the matrix
    for(int i = d -2; i >= 0; i--)
    {
       m[i][0] = (1-t) * m[i+1][0];

       for(int j = 1; j < d-i; j++)
       {
            m[i][j] = t * m[i+1][j-1] + (1-t) * m[i+1][j];
       }
       m[i][d-i] = t * m[i+1][d-i-1];
    }


    std::cout << std::endl;
    std::cout << m << std::endl;

    m[d][0] = -delta;
    m[d][1] = delta;
    //Calculates the bottom half of the matrix.
    for(int k = 2; k <= d; k++)
    {
        for(int i = d; i > d-k; i--)
        {
            m[i][k] = k * m[i][k-1];
            for(int j = k-1; j > 0; j--)
            {
                m[i][j] = k * (m[i][j-1] - m[i][j]);
            }
            m[i][0] = - k * m[i][0];
        }
    }
}
