#ifndef MYWATERSURFACE_H
#define MYWATERSURFACE_H

#include "myerbssurf.h"

class MyWaterSurface : public MyERBSSurf<float>
{
public:
    MyWaterSurface(float waveHeight, float waveWidth, int sampleU, int sampleV);

protected:
    void localSimulate(double dt);

private:
    float _waveHeight, _waveWidth;
};

#endif // MYWATERSURFACE_H
