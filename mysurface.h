#ifndef MYSURFACE_H
#define MYSURFACE_H

#include "gmParametricsModule"

class MySurface : public GMlib::PSurf<float, 3>
{
    GM_SCENEOBJECT(MySurface)
public:
    MySurface(float tail, float head);
    MySurface(const MySurface &mysurface);


protected:
    float getStartPU();
    float getEndPU();
    float getStartPV();
    float getEndPV();

    bool isClosedU() const;
    bool isClosedV() const;

    void eval(float u, float v, int d1, int d2, bool lu, bool lv );

private:
    float _tailSize;
    float _headSize;
};

#endif // MYSURFACE_H
