
#pragma once

#include <cassert>

#include <vector>
using std::vector;
#include <set>
using std::set;

#include <optional>
using std::nullopt;

#include "minisat.hxx"
using namespace Minisat;

#include "mana.hxx"
#include "bf.hxx"
#include "aig.hxx"

#include "aig_to_bf.hxx"
#include "bf_to_minisat.hxx"

//=================================================================================================
// Aig => Minisat
//
// add `aig` as a new state and return map from aig to minisat
vector<Var> addAig (const Aig& aig, Mana& m);
// add aig with state vars determined by `last_varmap` and return map from aig to minisat
vector<Var> extendAig (const vector<Var>& last_varmap, const Aig& aig, Mana& m);
// add aig with state vars determined by `last_varmap` selectively on
vector<Var> extendAigSel (const set<AigVar>& cone, const vector<Var>& last_varmap, const Aig& aig, Mana& m);
