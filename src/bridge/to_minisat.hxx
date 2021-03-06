
#pragma once

#include <cassert>

#include <vector>
#include <map>
using std::map;

#include <optional>
using std::optional; using std::nullopt;

#include "minisat.hxx"
using namespace Minisat;

#include "mana.hxx"
#include "bf.hxx"
#include "aig.hxx"

//=================================================================================================
// Bf => Minisat
//
// import bf and return formula root; base nodes must already be in m
Var addBf (const Bf_ptr& bf, Mana& m, optional<Sw> swopt=nullopt);

// handy queries
bool sat  (const Bf_ptr& bf, Mana& m);
bool hold (const Bf_ptr& bf, Mana& m);

//=================================================================================================
// Aig => Minisat
//
// add aig as a new state and return map from aig to minisat
map<AigVar,Var> addAig (const Aig& aig, Mana& m);
