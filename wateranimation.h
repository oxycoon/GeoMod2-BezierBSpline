#ifndef WATERANIMATION_H
#define WATERANIMATION_H

#include "animation.h"

class WaterAnimation : public Animation
{
public:
    WaterAnimation(float height = 1.0f, float length = 1.0f);

    void runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3>* > &surfaces, double dt);

private:
    float _waveHeight, _waveLength;

};

#endif // WATERANIMATION_H
