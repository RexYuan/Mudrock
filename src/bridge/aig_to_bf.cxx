
#include "aig_to_bf.hxx"

//=================================================================================================
// Aig types => Bf
//
Bf_ptr toBf (const AigVar& aigvar)
{
    return v(static_cast<int>(aigvar));
}

Bf_ptr toBf (const AigLit& aiglit)
{
    return aiglit.sign ? toBf(aiglit.var) : ~toBf(aiglit.var);
}
