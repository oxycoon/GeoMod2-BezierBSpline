#include "wateranimation.h"

WaterAnimation::WaterAnimation(float height, float length):
    _waveHeight(height), _waveLength(length)
{
    _time = 0;
}

void WaterAnimation::runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3>* > &surfaces, double dt)
{
    int xSize = surfaces.getDim1();
    int ySize = surfaces.getDim2();
    _time += dt;

    /*float piOverX = 3.14f / (xSize*2);
    float piOverY = 3.14f / (ySize*2);*/

    for(int i = 0; i < xSize; i++)
    {
        for(int j = 0; j < ySize; j++)
        {
            GMlib::PSurf<float,3>* surf = surfaces[i][j];

            GMlib::Vector<float,3> translate;
            float zTranslate = sin(_waveLength * (float)(i/*piOverX*/) + _time) * cos(_waveLength * (float)(j/*piOverY*/) + _time) * _waveHeight;

            float u = surf->getParStartU() + (surf->getParEndU() - surf->getParStartU())/2.0f;
            float v = surf->getParStartV() + (surf->getParEndV() - surf->getParStartV())/2.0f;
            surf->evaluate(u, v, 1, 1);
            translate = GMlib::Vector<float,3>(0.0f, 0.0f, zTranslate);
            //translate.normalize();

            translate *= dt;

            surf->translate(translate);
        }
    }
}
