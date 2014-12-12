#include "mywatersurface.h"

MyWaterSurface::MyWaterSurface(float waveHeight, float waveWidth, int sampleU, int sampleV):
    _waveHeight(waveHeight), _waveWidth(waveWidth),MyERBSSurf<float>(new GMlib::PPlane<float>(
        GMlib::Point<float,3>(0.0f, 0.0f, 0.0f), GMlib::Vector<float,3>(10.0f, 0.0f, 0.0f),
        GMlib::Vector<float,3>(0.0f, 0.0f, 10.0f)), sampleU, sampleV, 2, 2, LocalSurfaceType::SUBSURFACE )
{
}

void MyWaterSurface::localSimulate(double dt)
{
    int xSize = _c.getDim1();
    int ySize = _c.getDim2();




    /*for(int x = 0; x < xSize; x++)
    {
        for(int y = 0; y < ySize; y++)
        {
            float waveTranslation = (float)(sin(_waveWidth * (float)(x/10) * dt) *
                                            cos(_waveWidth * (float)(y/10) * dt)) * _waveHeight;

            _c[x][y]->translate(GMlib::Vector<float,3>(0.0f, 0.0f ,waveTranslation));
        }
    }*/
    this->replot();
}
