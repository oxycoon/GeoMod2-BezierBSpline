#ifndef MYSUBSURFACE_H
#define MYSUBSURFACE_H

#include <parametrics/gmpsurf>

template<typename T>
class MySubSurface: public GMlib::PSurf<T,3>
{
    GM_SCENEOBJECT(MySubSurface)
public:
    MySubSurface();
    MySubSurface(const MySubSurface &copy);
    MySubSurface(GMlib::PSurf<T,3>* c, T startU, T endU, T startV, T endV, T u, T v, bool closedU = false, bool closedV = false);
    virtual ~MySubSurface();

    bool isClosedU() const;
    bool isClosedV() const;
protected:
    GMlib::PSurf<T,3>* _c;
    GMlib::Vector<T,3> _translation;

    bool _isClosedU;
    bool _isClosedV;

    T _startU;
    T _startV;
    T _endU;
    T _endV;
    T _u;
    T _v;

    void eval(T u, T v, int d1, int d2, bool lu, bool lv);

    T getStartPU();
    T getStartPV();
    T getEndPU();
    T getEndPV();


private:
    void set(GMlib::PSurf<T,3>* c, T startU, T endU, T startV, T endV, T u, T v, bool closedU, bool closedV);
};

#include "mysubsurface.c"
#endif // MYSUBSURFACE_H
