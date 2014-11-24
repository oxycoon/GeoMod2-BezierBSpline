#include "knotvector.h"

template<typename T>
inline
KnotVector<T>::KnotVector()
{
    _delta = 0;
}

template<typename T>
inline
void KnotVector<T>::setDelta(T delta)
{
    _delta = delta;
}

template<typename T>
inline
T KnotVector<T>::getKnotValue(int index) const
{
    return (*this)(index);
}
