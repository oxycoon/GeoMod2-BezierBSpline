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

    for(int i = 0; i < 4; i++)
    {
        GMlib::PSurf<float,3> *surf = surfaces[2][i];
        float u = surf->getParStartU() + (surf->getParEndU() - surf->getParStartU())/2.0f;
        float v = surf->getParStartV() + (surf->getParEndV() - surf->getParStartV())/2.0f;
        surf->evaluate(u, v, 1, 1);
        surf->translate(translate/4);
    }

    for(int i = 0; i < 4; i++)
    {
        GMlib::PSurf<float,3> *surf = surfaces[3][i];
        float u = surf->getParStartU() + (surf->getParEndU() - surf->getParStartU())/2.0f;
        float v = surf->getParStartV() + (surf->getParEndV() - surf->getParStartV())/2.0f;
        surf->evaluate(u, v, 1, 1);
        surf->translate(translate);
    }
}
