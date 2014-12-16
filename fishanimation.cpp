#include "fishanimation.h"

FishAnimation::FishAnimation()
{
}

void FishAnimation::runAnimation(GMlib::DMatrix<GMlib::PSurf<float,3> *> &surfaces, double dt)
{
    int xSize = surfaces.getDim1();
    int ySize = surfaces.getDim2();
    _time += dt;


    float moveGradient = 0.5f;

    /*for(int i = 0; i < xSize; i++)
    {
        for(int j = 0; j < ySize-1; j++)
        {
            GMlib::Vector<float,3> translate;

            float swimMovement = cos(moveGradient + _time);
            translate = GMlib::Vector<float,3>(0.0f, swimMovement, 0.0f);

            translate *= 0.01f;

            GMlib::PSurf<float,3>* surf = surfaces[i][j];


            //surf->rotate(GMlib::Angle(moveGradient*0.01), GMlib::Vector<float,3>(0.0f, 1.0f, 0.0f));
            surf->translate(translate);
        }
        moveGradient += 1.0f/xSize;
    }*/

    GMlib::Vector<float,3> translate;

    float swimMovement = cos(_time);
    translate = GMlib::Vector<float,3>(0.0f, swimMovement, 0.0f);

    translate *= 0.01f;

    /*surfaces[1][0]->translate(translate/4);
    surfaces[1][1]->translate(translate/4);
    surfaces[1][2]->translate(translate/4);
    surfaces[1][3]->translate(translate/4);*/

    surfaces[2][0]->translate(translate/4);
    surfaces[2][1]->translate(translate/4);
    surfaces[2][2]->translate(translate/4);
    surfaces[2][3]->translate(translate/4);

    surfaces[3][0]->translate(translate);
    surfaces[3][1]->translate(translate);
    surfaces[3][2]->translate(translate);
    surfaces[3][3]->translate(translate);
    //surf->translate(translate);

    /*
     * [3][0]
     * [3][1]
     * [3][2]
     * [3][3]
     */
}
