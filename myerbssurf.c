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
MyERBSSurf<T>::MyERBSSurf(GMlib::PSurf<T, 3> &original)
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
void MyERBSSurf<T>::localSimulate(double dt)
{

}

template<typename T>
void MyERBSSurf<T>::eval(T u, T v, int d1, int d2, bool lu, bool lv)
{
    int indexU = findKnotIndex(u, _u, lu);
    int indexV = findKnotIndex(v, _v, lv);

    //GMlib::DMatrix<GMlib::Vector<T,3>> matrix = find
}

//--------------------------------------------------
//      PROTECTED KNOT VECTOR FUNCTIONS
//--------------------------------------------------

/**
 * @brief MyERBSSurf<T>::makeKnotVector
 * @param vector Reference to the vector to create.
 * @param samples
 * @param dim
 * @param closed
 * @param start
 * @param end
 */
template<typename T>
inline
void MyERBSSurf<T>::makeKnotVector(KnotVector<T> &vector, int samples, int dim, bool closed, T start, T end)
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
        vector[0] = vector[1] - (vector[samples-1] - vector[samples -2]);
        vector[samples + order - 1] = vector[samples]+ (vector[2] - vector[1]);
    }

}

template<typename T>
inline
int MyERBSSurf<T>::findKnotIndex(T t, const KnotVector<T> &vector, bool closed)
{
    int result, temp;
    if(closed)
    {
        temp = 2;
        result = 1;
    }
    else
    {
        temp = 3;
        result = vector.getDim() - 3;
    }

    for(int i = 1; i <= vector.getDim() - temp; i++)
    {
        float k = vector.getKnotValue(i);
        float k1 = vector.getKnotValue(i+1);

        if(t >= k && t < k1)
        {
            result = i;
            break;
        }
    }
    return result;
}

template<typename T>
inline
T MyERBSSurf<T>::mapKnot(T k, T start, T end)
{
    if(_localSurfaceType == SUBSURFACE)
    {
        return k;
    }
    else if(_localSurfaceType == BEZIERSURFACE)
    {
        return (k-start)/(end-start);
    }
}

template<typename T>
inline
void MyERBSSurf<T>::createSubSurfaces(GMlib::PSurf<T,3> *surf, int countU, int countV, bool closedU, bool closedV)
{

}


