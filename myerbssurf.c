#include "myerbssurf.h"

#include "mysubsurface.h"


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
MyERBSSurf<T>::MyERBSSurf(GMlib::PSurf<T, 3> *original)
{
    _surface = original;
}

template<typename T>
MyERBSSurf<T>::~MyERBSSurf()
{
    delete _surface;
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
    this->_p.setDim(d1+1, d2+1);

    int indexU = findKnotIndex(u, _u, lu);
    int indexV = findKnotIndex(v, _v, lv);

    T b1, b1d;
    T b2, b2d;

    _evaluator.set(_u[indexU], _u[indexU+1] - _u[indexU] );
    b1 = _evaluator(u);
    b1d = _evaluator.getDer1();
    //USE FOR U

    _evaluator.set(_v[indexV], _v[indexV+1] - _v[indexV] );
    b2 = _evaluator(u);
    b2d = _evaluator.getDer1();
    //USE FOR V

    GMlib::DVector<T> bu, bv, bud, bvd;
    bu.setDim(2); bv.setDim(2); bud.setDim(2); bvd.setDim(2);
    bu[0] = b1; bu[1] = 1-b1d;
    bv[0] = b2; bu[1] = 1-b2d;
    bud[0] = -b1d; bud[1] = b1d;
    bvd[0] = -b2d; bvd[1] = b2d;

    GMlib::DMatrix<GMlib::Vector<T,3>> s, su, sv;

    //GET LOCAL SURFACES
    // s, su, sv = evaluate(u,v,1,1)

    _surface->evaluate(u, v, 1, 1);


    this->_p[0][0] = bv * s ^ bu;
    this->_p[0][1] = bv * s ^ bud + bv * su ^ bu;
    this->_p[1][0] = bvd * s ^ bu + bv * sv ^ bu;

    /*
        bv * s * bu


    */
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
 * @param starut
 * @param end
 */
template<typename T>
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
/**
 * @brief MyERBSSurf<T>::makeBVector
 * @param bVector Reference to the vector which is being created
 * @param k Knot vector to use
 * @param knotIndex Index of the item
 * @param t
 * @param d
 *
 *  Create B vector for the given knot at given index.
 */
void MyERBSSurf<T>::makeBVector(GMlib::DVector<T> &bVector, const KnotVector<T> &k, int knotIndex, T t, int d)
{
    bVector.setDim(d+1);
    _evaluator.set(k.getKnotValue(knotIndex), k.getKnotValue(knotIndex+1) - k.getKnotValue(knotIndex));

    bVector[0] = _evaluator(t);
    bVector[1] = _evaluator.getDer1();
    bVector[2] = _evaluator.getDer2();
}

/**
 * @brief MyERBSSurf<T>::findKnotIndex
 * @param t Value to find index for
 * @param vector Knot vector to search in
 * @param closed Is knot vector closed
 * @return
 *
 *  Find the index for element in knot vector.
 */
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

/**
 * @brief MyERBSSurf<T>::mapKnot
 * @param k value to map
 * @param start Start knot
 * @param end End knot
 * @return
 *
 *  Maps the knots for bezier or just returns k if sub surface.
 */
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

/**
 * @brief MyERBSSurf<T>::createSubSurfaces
 * @param surf Surface to create the subsurfaces from
 * @param countU Amount of U knots
 * @param countV Amount of V knots
 * @param closedU Is U knots closed
 * @param closedV Is V knots closed
 *
 *  Creates sub surfaces for the c matrix based on the given surface.
 */
template<typename T>
void MyERBSSurf<T>::createSubSurfaces(GMlib::PSurf<T,3> *surf, int countU, int countV, bool closedU, bool closedV)
{
    _c.setDim(countU, countV);


    for(int i = 1; i < countU + 1; i++)
    {
        for(int j = 1; j < countV + 1; j++)
        {
            if(closedU && i == countU)
            {
                _c[i-1][j-1] = _c[0][j-1];
            }
            else if(closedV && j == countV)
            {
                _c[i-1][j-1] = _c[i-1][0];
            }
            else
            {
                GMlib::PSurf<T,3> *sub;

                sub = new MySubSurface<T>(surf, _u.getKnotValue(i-1), _u.getKnotValue(i+1),
                                       _v.getKnotValue(j-1), _v.getKnotValue(j+1),
                                       _u.getKnotValue(i), _v.getKnotValue(i));
                _c[i-1][j-1] = sub;
            }
        }
    }
}


