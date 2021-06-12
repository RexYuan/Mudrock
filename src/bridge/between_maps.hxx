
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
// (Aig => Minisat) map => Bf map
//
map<int,int> toBfmap (const map<AigVar,Var>& varmap);

//=================================================================================================
// Bf map => (Aig => Minisat) map
//
map<AigVar,Var> toAigmap (const map<int,int>& varmap);
