#include "myerbssurf.h"

#include "mysubsurface.h"
#include "mbeziersurface.h"

//--------------------------------------------------
//      CONSTRUCTORS
//--------------------------------------------------
template<typename T>
inline
MyERBSSurf<T>::MyERBSSurf()
{
}//end constructor

/**
 * @brief MyERBSSurf<T>::MyERBSSurf
 * @param original Original surface to turn into ERBSSurface
 * @param sampleU
 * @param sampleV
 * @param dim1
 * @param dim2
 */
template<typename T>
inline
MyERBSSurf<T>::MyERBSSurf(GMlib::PSurf<T, 3> *original, int sampleU, int sampleV, int dim1, int dim2, LocalSurfaceType type):
    _surface(original), _bezierDegree1(dim1), _bezierDegree2(dim2), _localSurfaceType(type)
{
    if(_surface->isClosedU())
        sampleU++;
    if(_surface->isClosedV())
        sampleV++;

    makeKnotVector(_u, sampleU, 1, _surface->isClosedU(), _surface->getParStartU(), _surface->getParEndU());
    makeKnotVector(_v, sampleV, 1, _surface->isClosedV(), _surface->getParStartV(), _surface->getParEndV());
    createSubSurfaces(_surface, sampleU, sampleV, _surface->isClosedU(), _surface->isClosedV());

    this->translate(GMlib::Vector<float,3>(0.0f, 0.0f, 1.0f));

    std::cout << "Amount of control points: " << std::endl;
}//End constructor

template<typename T>
MyERBSSurf<T>::~MyERBSSurf()
{
    delete _surface;
}//End deconstructor

template<typename T>
void MyERBSSurf<T>::localSimulate(double dt)
{
    //this->localSimulate(dt);

        /*1.
        2. p, u, v, uu, vv, uv, uuv, uvv, uuvv - 2nd derivative for bezier at least
        3. Decide movement
          - rotate
          - translate
          - scale
        4. optimalize
        5. replot at endx!*/

    _c[1][0]->rotate( GMlib::Angle(90) * dt, GMlib::Vector<float,3>(1.0f, 0.0f, 0.0f));

    _c[1][0]->replot();
    //this->rotate( GMlib::Angle(90) * dt, GMlib::Vector<float,3>(0.0f, 0.0f, 1.0f));

    //this->replot();

}

//--------------------------------------------------
//      INHERITED FUNCTIONS
//--------------------------------------------------
template<typename T>
inline
T MyERBSSurf<T>::getStartPU()
{
    return _surface->getParStartU();
}//End getStartPU()

template<typename T>
inline
T MyERBSSurf<T>::getStartPV()
{
    return _surface->getParStartV();
}//End getStartPV()

template<typename T>
inline
T MyERBSSurf<T>::getEndPU()
{
    return _surface->getParEndU();
}//End getEndPU()

template<typename T>
inline
T MyERBSSurf<T>::getEndPV()
{
    return _surface->getParEndV();
}//End getEndPV()

template<typename T>
void MyERBSSurf<T>::eval(T u, T v, int d1, int d2, bool lu, bool lv)
{
    this->_p.setDim(d1+1, d2+1);

    int indexU = findKnotIndex(u, _u, lu);
    int indexV = findKnotIndex(v, _v, lv);

    GMlib::DVector<T> b1, b2;

    makeBVector(b1, _u, indexU, u, d1); //B vector for u knot
    makeBVector(b2, _v, indexV, v, d2); //B vector for v knot

    GMlib::DVector<T> bu, bv, bud, bvd;
    bu.setDim(2); bv.setDim(2); bud.setDim(2); bvd.setDim(2);
    bu[1] = b1[0]; bu[0] = 1-b1[0];
    bv[1] = b2[0]; bv[0] = 1-b2[0];
    bud[0] = -b1[1]; bud[1] = b1[1];
    bvd[0] = -b2[1]; bvd[1] = b2[1];

    GMlib::DMatrix<GMlib::Vector<T,3> > s, su, sv;
    s.setDim(2,2); su.setDim(2,2); sv.setDim(2,2);


    //Sets the s, su and sv matrices, which contain positions and derivatives.
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            T mappedU = mapKnot(u, _u[indexU+i-1], _u[indexU+i+1]);
            T mappedV = mapKnot(v, _v[indexV+j-1], _v[indexV+j+1]);

            GMlib::DMatrix<GMlib::Vector<T,3> > tempS = _c[indexU + i - 1][indexV + j - 1]->evaluateParent(mappedU, mappedV, 1, 1);
            s[i][j] = tempS[0][0];
            su[i][j] = tempS[1][0];
            sv[i][j] = tempS[0][1];
        }//End for j
    }//End for i

    s.transpose();
    su.transpose();
    sv.transpose();

    this->_p[0][0] = bv * (s ^ bu);
    this->_p[0][1] = bv * (s ^ bud) + bv * (su ^ bu);
    this->_p[1][0] = bvd * (s ^ bu) + bv * (sv ^ bu);
}//End eval()

//--------------------------------------------------
//      PROTECTED KNOT VECTOR FUNCTIONS
//--------------------------------------------------

/**
 * @brief MyERBSSurf<T>::makeKnotVector
 * @param vector Reference to the vector to create.
 * @param samples Amount of knots
 * @param dim Dimension
 * @param closed Is vector closed?
 * @param start Starting T
 * @param end End T
 *
 *  Creates a knot vector with given parameters.
 */
template<typename T>
void MyERBSSurf<T>::makeKnotVector(KnotVector<T> &vector, int samples, int dim, bool closed, T start, T end)
{
    T delta = (end - start) / (samples - 1);

    int order = dim + 1;
    vector.setDim(samples + order); //Sets the dimension for the knot vector
    vector.setDelta(delta);

    int stepKnots = samples - order;

    for(int i = 0; i < order; i++)
    {
        vector[i] = start;
    }//End for i

    for(int i = 0; i < stepKnots; i++)
    {

        vector[i + order] = start + (i+1) * vector.getDelta();
    }//End for i

    for(int i = 0; i < order; i++)
    {
        vector[samples + i] = end;
    }//End for i

    if(closed)
    {
        vector[0] = vector[1] - vector.getDelta();
        vector[samples + order - 1] = vector[samples]+ vector.getDelta();
    }//End if
}//end makeKnotVector()

template<typename T>
inline
/**
 * @brief MyERBSSurf<T>::makeBVector
 * @param bVector Reference to the vector which is being created
 * @param k Knot vector to use
 * @param knotIndex Index of the item
 * @param t
 * @param d dimension
 *
 *  Create B vector for the given knot at given index.
 *      [0] - Value
 *      [1] - 1st derivative
 *      [2] - 2nd derivative (NOT IMPLEMENTED)
 */
void MyERBSSurf<T>::makeBVector(GMlib::DVector<T> &bVector, const KnotVector<T> &k, int knotIndex, T t, int d)
{
    //bVector.setDim(d+1);
    bVector.setDim(2);
    _evaluator.set(k.getKnotValue(knotIndex), k.getDelta());

    bVector[0] = _evaluator(t);
    bVector[1] = _evaluator.getDer1();
    //bVector[2] = _evaluator.getDer2();
}//end makeBVector()

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
    }//End if
    else
    {
        temp = 3;
        result = vector.getDim() - 3;
    }//End else

    for(int i = 1; i <= vector.getDim() - temp; i++)
    {
        float k = vector.getKnotValue(i);
        float k1 = vector.getKnotValue(i+1);

        if(t >= k && t < k1)
        {
            result = i;
            break;
        }//End if
    }//End for i
    return result;
}//End findKnotIndex()

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
    }//End if
    else if(_localSurfaceType == BEZIERSURFACE)
    {
        return (k-start)/(end-start);
    }//End else
}//End mapKnot()

/**
 * @brief MyERBSSurf<T>::makeCMatrix
 * @param u
 * @param v
 * @param uIndex
 * @param vIndex
 * @param d1
 * @param d2
 * @return
 *
 *  Creates the matrix of control points for the local surfaces.
 */
template <typename T>
GMlib::DMatrix<GMlib::Vector<T, 3> > MyERBSSurf<T>::makeCMatrix(T u, T v, int uIndex, int vIndex, int d1, int d2)
{
    //TODO: Create for bezier, this is just for sub surface
    GMlib::DMatrix<GMlib::Vector<T,3> > temp;
    GMlib::DMatrix<GMlib::Vector<T,3> > result;

    temp = _c[uIndex-1][vIndex-1]->evaluateParent(u, v, d1, d2);
    result = _c[uIndex][vIndex-1]->evaluateParent(u, v, d1, d2);

    GMlib::DVector<double> a, b;
    a.setDim(d1+1);

    makeBVector(b, _u, uIndex, u, d1);

    temp -= result;

    for(int i = 0; i <= d1; i++)
    {
        a[i] = 1;
        for(int j = i-1; j > 0; j--)
        {
            a[j] += a[j-1];
        }//End for j

        for(int j = 0; j <= i; j++)
        {
            result[i] = (a[j] * b[j]) * temp[i-j];
        }//End for j
    }//End for i
    return result;
}//End makeCMatrix()

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

    GMlib::Matrix<GMlib::Material, 2, 2> matMat;
    matMat[0][0] = GMlib::GMmaterial::Gold;
    matMat[1][0] = GMlib::GMmaterial::Ruby;
    matMat[0][1] = GMlib::GMmaterial::Sapphire;
    matMat[1][1] = GMlib::GMmaterial::Silver;

    for(int i = 1; i <= countU; i++)
    {
        for(int j = 1; j <= countV; j++)
        {
            if(closedU && i == countU)
            {
                _c[i-1][j-1] = _c[0][j-1];
            }//end if
            else if(closedV && j == countV)
            {
                _c[i-1][j-1] = _c[i-1][0];
            }//end else if
            else
            {
                GMlib::PSurf<T,3> *sub;
                if(_localSurfaceType == LocalSurfaceType::SUBSURFACE)
                {


                    sub = new MySubSurface<T>(surf, _u.getKnotValue(i-1), _u.getKnotValue(i+1),
                                           _v.getKnotValue(j-1), _v.getKnotValue(j+1),
                                           _u.getKnotValue(i), _v.getKnotValue(j));

                }//End if
                else if(_localSurfaceType == LocalSurfaceType::BEZIERSURFACE)
                {
                    sub = new MBezierSurface<T>(surf, _u.getKnotValue(i-1), _u.getKnotValue(i+1),
                                             _v.getKnotValue(j-1), _v.getKnotValue(j+1),
                                             _u.getKnotValue(i), _v.getKnotValue(j),
                                             _bezierDegree1, _bezierDegree2);
                }//End else if
                _c[i-1][j-1] = sub;
            }//End else
        }//End for j
    }//End for i
}//End creatSubSurfaces()


