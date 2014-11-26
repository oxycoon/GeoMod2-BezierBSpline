#include "mybspline.h"

template<typename T>
inline
MyBSpline<T>::MyBSpline()
{
}

template<typename T>
void MyBSpline<T>::eval(T u, T v, int d1, int d2, bool lu, bool lv)
{
    this->_p.setDim(d1+1, d2+1);



}

template<typename T>
int MyBSpline<T>::findKnotIndex(T t, const KnotVector<T> &vector, bool closed)
{
    int result, temp;
    if(closed)
    {
        temp = 2;
        result = 1;
    }
    else
    {
        temp = 3;
        result = vector.getDim() - 3;
    }

    for(int i = 1; i <= vector.getDim() - temp; i++)
    {
        float k = vector.getKnotValue(i);
        float k1 = vector.getKnotValue(i+1);

        if(t >= k && t < k1)
        {
            result = i;
            break;
        }
    }
    return result;
}
