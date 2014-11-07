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
void MBezierSurface<T>::evaluate(GMlib::DMatrix<T> &matrix, int d, T t, T delta)
{
    eval(matrix, d, t, delta);
}

template<typename T>
inline
void MBezierSurface<T>::eval(GMlib::DMatrix<T> &m, int d, T t, T delta)
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
    else
    {
        m.setDim(dim, dim);
    }

    m[d-1][0] = 1-t;
    m[d-1][1] = t;

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

        std::cout << std::endl;
        std::cout << m << std::endl;
    }


//    std::cout << std::endl;
//    std::cout << m << std::endl;

   T verifier;
    for(int i = 0; i <= d; i++)
    {
        for(int j = 0; j <= d; j++)
        {
            verifier += m[i][j];
        }
        std::cout << "Verifying line #" << i << " before derivatives: " << verifier << std::endl;
        verifier = 0;
    }


}
