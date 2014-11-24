#include "myerbssurf.h"

//--------------------------------------------------
//      CONSTRUCTORS
//--------------------------------------------------
template<typename T>
inline
MyERBSSurf<T>::MyERBSSurf()
{
}

template<typename T>
inline
MyERBSSurf::MyERBSSurf(GMlib::PSurf<T, _Tp2> &original)
{

}

//--------------------------------------------------
//      INHERITED FUNCTIONS
//--------------------------------------------------
template<typename T>
inline
T MyERBSSurf<T>::getStartPU()
{
    return _surface->getParStartU();
}

template<typename T>
inline
T MyERBSSurf<T>::getStartPV()
{
    return _surface->getParStartU();
}

template<typename T>
inline
T MyERBSSurf<T>::getEndPU()
{
    return _surface->getParEndV();
}

template<typename T>
inline
T MyERBSSurf<T>::getEndPV()
{
    return _surface->getParEndV();
}

template<typename T>
inline
void MyERBSSurf<T>::localSimulate(double dt)
{

}

template<typename T>
inline
void MyERBSSurf<T>::eval(T u, T v, int d1, int d2, bool lu, bool lv)
{

}

//--------------------------------------------------
//      PROTECTED KNOT VECTOR FUNCTIONS
//--------------------------------------------------
template<typename T>
inline
KnotVector MyERBSSurf<T>::makeKnotVector(KnotVector &vector, int samples, int dim, bool closed, T start, T end)
{
    T delta = (end - start) /(samples-1);

    int order = dim + 1;
    vector.setDim(samples + order); //Sets the dimension for the knot vector

    int knotValue = 0;
    int stepKnots = samples - order;

    for(int i = 0; i < order; i++)
    {
        vector[i] = start + knotValue * delta;
    }

    for(int i = 0; i < stepKnots; i++)
    {
        knotValue++;
        vector[i + order] = start + knotValue * delta;
    }

    knotValue++;
    for(int i = 0; i < order; i++)
    {
        vector[samples + 1] = start + knotValue*delta;
    }

    if(closed)
    {
        vector
    }
}

template<typename T>
inline
int MyERBSSurf<T>::findKnotIndex(T t, const KnotVector &theVector, bool closed)
{

}

template<typename T>
inline
float MyERBSSurf<T>::mapKnot(T u, T start, T end)
{

}


