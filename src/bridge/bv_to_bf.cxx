
#include "bv_to_bf.hxx"

//=================================================================================================
// Bv => Bf
//
Bf_ptr toBf (Bv* bv)
{
    Bf_ptr tmp = v(true);

    int i=0;
    for (auto bit=bv->begin(); bit!=bv->end(); bit++, i++)
        if (*bit)
            tmp = tmp & v(i);
        else
            tmp = tmp & ~v(i);

    return tmp;
}
