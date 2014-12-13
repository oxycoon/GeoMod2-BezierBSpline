#ifndef ANIMATION_H
#define ANIMATION_H

#include <gmParametricsModule>

class Animation
{
public:
    virtual ~Animation(){}

    virtual void runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3>* > &surfaces, double dt) = 0;

protected:
    double _time;
};

#endif // ANIMATION_H
