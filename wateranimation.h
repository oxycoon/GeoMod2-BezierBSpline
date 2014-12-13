#ifndef WATERANIMATION_H
#define WATERANIMATION_H

#include "animation.h"

class WaterAnimation : public Animation
{
public:
    WaterAnimation();

    void runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3>* > &surfaces, double dt);

};

#endif // WATERANIMATION_H
