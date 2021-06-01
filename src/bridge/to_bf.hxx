
#pragma once

#include <cassert>

#include <map>
using std::map;

#include "minisat.hxx"
using namespace Minisat;

#include "bf.hxx"
#include "bv.hxx"
#include "face.hxx"
#include "aig.hxx"

//=================================================================================================
// Bv => Bf
//
Bf_ptr toBf (const Bv& bv); // return the characteristic conjunction of `bv`; vars numbered from 0

//=================================================================================================
// Face => Bf
//
Bf_ptr toBf (const Face& face); // convert to dnf; note that vars are numbered from 0

//=================================================================================================
// Aig types => Bf
//
Bf_ptr toBf (const AigVar& aigvar);
Bf_ptr toBf (const AigLit& aiglit);

//=================================================================================================
// (Aig => Minisat) map => Bf map
//
map<int,int> toBfmap (const map<AigVar,Var>& varmap);

//=================================================================================================
// Bf map => (Aig => Minisat) map
//
map<AigVar,Var> toAigmap (const map<int,int>& varmap);
