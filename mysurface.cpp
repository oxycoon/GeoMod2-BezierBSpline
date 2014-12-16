#include "mysurface.h"

#define PI 3.14159265359

MySurface::MySurface(float tail, float head) : _tailSize(tail), _headSize(head)
{
    this->_dm = GMlib::GM_DERIVATION_EXPLICIT;
}//End constructor

MySurface::MySurface(const MySurface &mysurface) :PSurf<float,3>(mysurface)
{

}//End constructor

float MySurface::getStartPU()
{
    return 0.0f;
}//End getStartPU()

float MySurface::getEndPU()
{
    return 1.0f * PI;
}//End getEndPU()

float MySurface::getStartPV()
{
    return 0.0f;
}//End getStartPV()

float MySurface::getEndPV()
{
    return 2.0f * PI;
}//End getEndPV()

bool MySurface::isClosedU() const
{
    return false;
}//End isClosedU

bool MySurface::isClosedV() const
{
    return true;
}//End isClosedV()

void MySurface::eval(float u, float v, int d1, int d2, bool, bool)
{
    this->_p.setDim(d1+1, d2+1);

    float su = sin(u);
    float s2u = sin(2*u);
    float sv = sin(v);
    float cu = cos(u);
    float c2u = cos(2*u);
    float cv = cos(v);

    this->_p[0][0][0] = (cu - c2u) * cv / 4;
    this->_p[0][0][1] = (su + s2u) * sv / 4;
    this->_p[0][0][2] = cu;

    //DERIVATIVES WRONG? USING SUBSURFACE FOR NOW.
    if( this->_dm == GMlib::GM_DERIVATION_EXPLICIT )
    {
        if(d1)//du
        {
            this->_p[1][0][0] = (-su + (2 * s2u)) * cv / 4;
            this->_p[1][0][1] = (cu + (2 * c2u)) * sv / 4;
            this->_p[1][0][2] = -su;
        }//End if
        if(d1 > 1)//duu
        {
            this->_p[2][0][0] = (-cu + (4 * c2u)) * cv / 4;
            this->_p[2][0][1] = (-su - (4 * s2u)) * sv / 4;
            this->_p[2][0][2] = -cu;
        }//End if
        if(d2)//dv
        {
            this->_p[0][1][0] = (cu - c2u) * -sv / 4;
            this->_p[0][1][1] = (su + s2u) * cv / 4;
            this->_p[0][1][2] = 0.0f;
        }//End if
        if(d2>1)//dvv
        {
            this->_p[0][2][0] = (cu - c2u) * -cv / 4;
            this->_p[0][2][1] = (su + s2u) * -sv / 4;
            this->_p[0][2][2] = 0.0f;
        }//End if
        if(d1 && d2)//duv
        {
            this->_p[1][1][0] = (su - (2 * s2u)) * sv / 4;
            this->_p[1][1][1] = (cu + (2 * c2u)) * cv / 4;
            this->_p[1][1][2] = 0;
        }//End if
        if(d1 > 1 && d2)//duuv
        {
            this->_p[2][1][0] = (cu - (4 * c2u)) * sv / 4;
            this->_p[2][1][1] = (su + (4 * s2u)) * -cv / 4;
            this->_p[2][1][2] = 0;
        }//End if
        if(d1 && d2 > 1)//duvv
        {
            this->_p[1][2][0] = (cu - c2u) * cv / 4;
            this->_p[1][2][1] = (cu + c2u) * -sv / 4;
            this->_p[1][2][2] = 0;
        }//End if
        if(d1 > 1 && d2 > 1)//duuvv
        {
            this->_p[2][2][0] = (cu - (4 * c2u)) * cv / 4;
            this->_p[2][2][1] = (su + (4 * s2u)) * sv / 4;
            this->_p[2][2][2] = 0;
        }//End if

        /*if(d1)//du
        {
            this->_p[1][0][0] = (-su + (2 * s2u)) * cv / 4;
            this->_p[1][0][1] = (cu - (2 * c2u)) * sv / 4;
            this->_p[1][0][2] = -su;
        }//End if
        if(d1 > 1)//duu
        {
            this->_p[2][0][0] = (-cu + (4 * c2u)) * cv / 4;
            this->_p[2][0][1] = (-su + (4 * s2u)) * sv / 4;
            this->_p[2][0][2] = -cu;
        }//End if
        if(d2)//dv
        {
            this->_p[0][1][0] = (cu - c2u) * -sv / 4;
            this->_p[0][1][1] = (su - s2u) * cv / 4;
            this->_p[0][1][2] = 0.0f;
        }//End if
        if(d2>1)//dvv
        {
            this->_p[0][2][0] = (cu - c2u) * -cv / 4;
            this->_p[0][2][1] = (su - s2u) * -sv / 4;
            this->_p[0][2][2] = 0.0f;
        }//End if
        if(d1 && d2)//duv
        {
            this->_p[1][1][0] = (su - (2 * s2u)) * sv / 4;
            this->_p[1][1][1] = (cu - (2 * c2u)) * cv / 4;
            this->_p[1][1][2] = 0;
        }//End if
        if(d1 > 1 && d2)//duuv
        {
            this->_p[2][1][0] = (cu - (4 * c2u)) * sv / 4;
            this->_p[2][1][1] = (su - (4 * s2u)) * -cv / 4;
            this->_p[2][1][2] = 0;
        }//End if
        if(d1 && d2 > 1)//duvv
        {
            this->_p[1][2][0] = (cu - c2u) * cv / 4;
            this->_p[1][2][1] = (cu - c2u) * -sv / 4;
            this->_p[1][2][2] = 0;
        }//End if
        if(d1 > 1 && d2 > 1)//duuvv
        {
            this->_p[2][2][0] = (cu - (4 * c2u)) * cv / 4;
            this->_p[2][2][1] = (su - (4 * s2u)) * sv / 4;
            this->_p[2][2][2] = 0;
        }//End if*/
    }//End if
}//End eval()
