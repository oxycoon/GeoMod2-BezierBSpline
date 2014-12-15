#ifndef FISHANIMATION_H
#define FISHANIMATION_H

#include "animation.h"

class FishAnimation : public Animation
{
public:
    FishAnimation();

    void runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3>* > &surfaces, double dt);
};

#endif // FISHANIMATION_H
