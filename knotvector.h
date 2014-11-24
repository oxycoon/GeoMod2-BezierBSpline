#ifndef KNOTVECTOR_H
#define KNOTVECTOR_H

#include <gmCoreModule>

template<typename T>
class KnotVector : public GMlib::DVector<T>
{
public:
    KnotVector();

    void setDelta(T delta);
    T getKnotValue(int index) const;

private:
    T _delta;
};

#include "knotvector.c"

#endif // KNOTVECTOR_H
