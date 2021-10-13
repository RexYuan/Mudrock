
#include "bv_to_bf.hxx"

//=================================================================================================
// Bv => Bf
//
Bf_ptr toBf (const Bv_ptr bv)
{
    Bf_ptr tmp = conj();
    tmp->reserve(bv->len());

    int i=0;
    for (auto bit=bv->begin(); bit!=bv->end(); bit++, i++)
        if (*bit)
            tmp += v(i);
        else
            tmp += ~v(i);

    return tmp;
}
