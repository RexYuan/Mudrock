
#include "between_maps.hxx"

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
