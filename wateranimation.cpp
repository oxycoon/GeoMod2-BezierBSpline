#include "wateranimation.h"

WaterAnimation::WaterAnimation()
{
    _time = 0;
}

void WaterAnimation::runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3>* > &surfaces, double dt)
{
    int xSize = surfaces.getDim1();
    int ySize = surfaces.getDim2();
    _time += dt;

    for(int i = 0; i < xSize; i++)
    {
        for(int j = 0; j < ySize; j++)
        {
            GMlib::PSurf<float,3>* surf = surfaces[i][j];

            GMlib::Vector<float,3> translate;

            float u = surf->getParStartU() + (surf->getParEndU() - surf->getParStartU())/2.0f;
            float v = surf->getParStartV() + (surf->getParEndV() - surf->getParStartV())/2.0f;
            surf->evaluate(u, v, 1, 1);
            translate = GMlib::Vector<float,3>(0.0f, 0.0f, 1.0f);
            translate.normalize();

            translate *= dt;

            surf->translate(translate);
        }
    }

}
