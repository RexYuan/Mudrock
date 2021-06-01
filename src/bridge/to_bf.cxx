
#include "to_bf.hxx"

//=================================================================================================
// Bv => Bf
//
Bf_ptr toBf (const Bv& bv)
{
    Bf_ptr tmp = v(true);

    int i=0;
    for (auto bit=bv.begin(); bit!=bv.end(); bit++, i++)
        if (*bit)
            tmp = tmp & v(i);
        else
            tmp = tmp & ~v(i);

    return tmp;
}

//=================================================================================================
// Face => Bf
//
Bf_ptr toBf (const Face& face)
{
    Bf_ptr disj = v(false);

    Bf_ptr clus; int i;
    for (const Bv& term : face.primes())
    {
        clus = v(true);

        i = 0; // bf vars are mapped to bv index
        for (auto bit=face.basis().begin(), tit=term.begin();
             bit != face.basis().end(); ++bit, ++tit, ++i)
        {
            if (!*bit && *tit)
                clus = clus & v(i);
            else if (*bit && !*tit)
                clus = clus & ~v(i);
        }

        disj = disj | clus;
    }
    return disj;
}

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

//=================================================================================================
// (Aig => Minisat) map => Bf map
//
map<int,int> toBfmap (const map<AigVar,Var>& varmap)
{
    map<int,int> tmp;
    for (const auto& [aigvar,var] : varmap)
        tmp[static_cast<int>(aigvar)] = static_cast<int>(var);
    return tmp;
}

//=================================================================================================
// Bf map => (Aig => Minisat) map
//
map<AigVar,Var> toAigmap (const map<int,int>& varmap)
{
    map<AigVar,Var> tmp;
    for (const auto& [aigvar,var] : varmap)
        tmp[static_cast<AigVar>(aigvar)] = static_cast<Var>(var);
    return tmp;
}
