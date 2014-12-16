#include "fishanimation.h"

FishAnimation::FishAnimation()
{
}

void FishAnimation::runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3> *> &surfaces, double dt)
{
    _time += dt;

    GMlib::Vector<float,3> translate;

    float swimMovement = cos(_time);
    translate = GMlib::Vector<float,3>(0.0f, swimMovement, 0.0f);

    translate *= 0.01f;

    surfaces[2][0]->translate(translate/4);
    surfaces[2][1]->translate(translate/4);
    surfaces[2][2]->translate(translate/4);
    surfaces[2][3]->translate(translate/4);

    surfaces[3][0]->translate(translate);
    surfaces[3][1]->translate(translate);
    surfaces[3][2]->translate(translate);
    surfaces[3][3]->translate(translate);
}
