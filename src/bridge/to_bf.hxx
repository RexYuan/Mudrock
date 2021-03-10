
#pragma once

#include <cassert>

#include <map>
using std::map;

#include "minisat.hxx"
using namespace Minisat;

#include "bf.hxx"
#include "face.hxx"
#include "aig.hxx"

//=================================================================================================
// Face => Bf
//
Bf_ptr toBf (const Face& face);

//=================================================================================================
// Aig types => Bf
//
Bf_ptr toBf (const AigVar& aigvar);
Bf_ptr toBf (const AigLit& aiglit);

//=================================================================================================
// (Aig => Minisat) map => Bf map
//
map<int,int> toBfmap (const map<AigVar,Var>& varmap);
