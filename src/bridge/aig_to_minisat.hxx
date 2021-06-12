
#pragma once

#include <cassert>

#include <map>
using std::map;

#include <optional>
using std::nullopt;

#include "minisat.hxx"
using namespace Minisat;

#include "mana.hxx"
#include "bf.hxx"
#include "aig.hxx"
#include "to_bf.hxx"
#include "bf_to_minisat.hxx"

//=================================================================================================
// Aig => Minisat
//
// add `aig` as a new state and return map from aig to minisat
map<AigVar,Var> addAig (const Aig& aig, Mana& m);
// add aig with state vars determined by `last_varmap` and return map from aig to minisat
map<AigVar,Var> addAig (const map<AigVar,Var>& last_varmap, const Aig& aig, Mana& m);
