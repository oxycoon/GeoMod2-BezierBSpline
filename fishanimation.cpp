#include "fishanimation.h"

FishAnimation::FishAnimation()
{
}

void FishAnimation::runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3> *> &surfaces, double dt)
{
    int xSize = surfaces.getDim1();
    int ySize = surfaces.getDim2();
    _time += dt;


    float moveGradient = 0.0f;
    GMlib::Vector<float,3> translate;

    float swimMovement = cos(/*moveGradient + */ _time);
    translate = GMlib::Vector<float,3>(swimMovement, 0.0f, 0.0f);

    translate *= 0.01f;
    for(int i = 0; i < xSize; i++)
    {
        for(int j = 0; j < ySize-1; j++)
        {
            GMlib::PSurf<float,3>* surf = surfaces[i][j];

            surf->translate(translate);
        }
        //moveGradient += 1.0f/xSize;
    }
}
