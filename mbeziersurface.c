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
MBezierSurface<T>::MBezierSurface(GMlib::DMatrix<GMlib::Vector<T,3> > c): _c(c)
{

}

template<typename T>
inline
MBezierSurface<T>::MBezierSurface(GMlib::PSurf<T,3> *surf, T startU, T endU, T startV, T endV, T u, T v,
                                  int dim1, int dim2, bool closedU, bool closedV):
    _originalSurface(surf), _startU(startU), _endU(endU), _startV(startV), _endV(endV), _u(u), _v(v),
    _dim1(dim1), _dim2(dim2), _isClosedU(closedU), _isClosedV(closedV)
{
    _deltaU = 1 / (_endU - _startU);
    _deltaV = 1 / (_endV - _startV);

    T scaledU = (u - startU) / (endU - startU);
    T scaledV = (v - startV) / (endV - startV);

    computeControlPoints(_originalSurface, u, v, scaledU, scaledV);
}

template<typename T>
inline
void MBezierSurface<T>::translate(const GMlib::Vector<T,3> &trans_vector)
{
    //std::cout << "Surf - " << this->getName() << " trans vector: " << trans_vector << std::endl;

    GMlib::Parametrics<T,2,3>::translate(trans_vector);
}

template<typename T>
void MBezierSurface<T>::eval(T u, T v, int d1, int d2, bool, bool)
{
    this->_p.setDim(d1 + 1, d2 + 1);

    GMlib::DMatrix<T> b1;
    GMlib::DMatrix<T> b2;

    computeBMatrix(b1, _c.getDim1()-1, u);
    computeBMatrix(b2, _c.getDim2()-1, v);
    b2.transpose();

    this->_p = b1 * (_c ^ b2);
}

template<typename T>
inline
T MBezierSurface<T>::getStartPU()
{
    return 0;
}

template<typename T>
inline
T MBezierSurface<T>::getStartPV()
{
    return 0;
}

template<typename T>
inline
T MBezierSurface<T>::getEndPU()
{
    return 1;
}

template<typename T>
inline
T MBezierSurface<T>::getEndPV()
{
    return 1;
}

template<typename T>
inline
bool MBezierSurface<T>::isClosedU() const
{
    return _isClosedU;
}

template<typename T>
inline
bool MBezierSurface<T>::isClosedV() const
{
    return _isClosedV;
}

/**
 * @brief MBezierSurface<T>::computeBMatrix
 * @param m Matrix which is being computed
 * @param d order
 * @param t
 * @param delta
 *
 *  Computes the basis matrix for given matrix and values
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

    //Sets the first values in the matrix, Bertnstein poly.
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

    m[d][0] = -delta;
    m[d][1] = delta;
    //Calculates the bottom half of the matrix.
    for(int k = 2; k <= d; k++)
    {
        double scale = k * delta;
        for(int i = d; i > d-k; i--)
        {
            m[i][k] = scale * m[i][k-1];//Start of the row
            for(int j = k-1; j > 0; j--)
            {
                m[i][j] = scale * (m[i][j-1] - m[i][j]);//Middle of the row
            }
            m[i][0] = - scale * m[i][0]; //End of the row
        }
    }

//    std::cout << std::endl;
//    std::cout << m << std::endl;
}

template<typename T>
void MBezierSurface<T>::computeControlPoints(GMlib::PSurf<T,3> *surf, T u, T v, T uScaled, T vScaled)
{
    GMlib::DMatrix<GMlib::Vector<T,3> > temp = surf->evaluateParent(u, v, _dim1, _dim2);
    GMlib::DMatrix<T> bu, bv;

    computeBMatrix(bu, temp.getDim1()-1, uScaled, _deltaU);
    computeBMatrix(bv, temp.getDim2()-1, vScaled, _deltaV);
    bu.invert();
    bv.invert();
    bv.transpose();

    _c.setDim(temp.getDim1(), temp.getDim2());
    _c = bu * (temp ^ bv);

    for(int i = 0; i < temp.getDim1(); i++)
    {
        for(int j = 0; j < temp.getDim2(); j++)
        {
            _c[i][j] -= temp[0][0];
        }
    }
    this->translate(temp[0][0]);


    //std::cout << _c << std::endl;
}
