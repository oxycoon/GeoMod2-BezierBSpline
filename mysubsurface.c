#include "mysubsurface.h"

//--------------------------------------------------
//  CONSTRUCTORS
//--------------------------------------------------

/**
 * @brief MySubSurface<T>::MySubSurface
 *
 *  Empty constructor
 */
template <typename T>
inline
MySubSurface<T>::MySubSurface()
{
}

/**
 * @brief MySubSurface<T>::MySubSurface
 * @param copy
 *
 *  Copy constructor
 */
template <typename T>
inline
MySubSurface<T>::MySubSurface(const MySubSurface &copy)
{

}

/**
 * @brief MySubSurface<T>::MySubSurface
 * @param c
 * @param startU
 * @param endU
 * @param startV
 * @param endV
 * @param u
 * @param v
 *
 *  Constructor
 */
template <typename T>
inline
MySubSurface<T>::MySubSurface(GMlib::PSurf<T, _Tp2> *c, T startU, T endU, T startV, T endV, T u, T v, bool closedU, bool closedV)
{
    set(c, startU, endU, startV, endV, u, v, closedU, closedV);
    GMlib::DMatrix<GMlib::Vector<T,3>> temp = _c->evaluateParent(u, v, 0, 0);
    _translation = temp[0][0];
    this->translate(_translation);
}

//--------------------------------------------------
//      PUBLIC GET/SET
//--------------------------------------------------
/**
 * @brief MySubSurface::isClosedU
 * @return if the surface is closed on U
 */
template <typename T>
inline
bool MySubSurface<T>::isClosedU() const
{
    return _isClosedU;
}

/**
 * @brief MySubSurface::isClosedV
 * @return if the surface is closed on V
 */
template <typename T>
inline
bool MySubSurface<T>::isClosedV() const
{
    return _isClosedV;
}

//--------------------------------------------------
//      PROTECTED FUNCTIONS
//--------------------------------------------------
template <typename T>
inline
bool MySubSurface<T>::eval(T u, T v, int d1, int d2, bool lu, bool lv)
{
    this->_p = _c->evaluateParent(u, v, d1, d2);
    this->_p[0][0] -= _translation;
}

//--------------------------------------------------
//      PRIVATE SET
//--------------------------------------------------

/**
 * @brief MySubSurface<T>::set
 * @param c
 * @param startU
 * @param endU
 * @param startV
 * @param endV
 * @param u
 * @param v
 *
 *  Sets the object values to specified values, used with the constructors.
 */
template <typename T>
inline
void MySubSurface<T>::set(GMlib::PSurf<T, _Tp2> *c, T startU, T endU, T startV, T endV, T u, T v, bool closedU, bool closedV)
{
    _c = c;
    _startU = startU;
    _endU = endU;
    _startV = startV;
    _endV = endV;
    _u = u;
    _v = v;
    _isClosedU = closedU;
    _isClosedV = closedV;
}
