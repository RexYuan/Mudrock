
#pragma once

#include <cassert>

#include <string>
using namespace std::string_literals;
#include <map>
using std::map;

#include <optional>
using std::optional;
using std::nullopt;

#include "minisat.hxx"
using namespace Minisat;

#include "mana.hxx"
#include "bf.hxx"
#include "aig.hxx"
#include "to_bf.hxx"

//=================================================================================================
// Bf => Minisat
//
// import `bf` and return formula root; base nodes must already be in `m`
Var addBf (const Bf_ptr& bf, Mana& m, optional<Sw> swopt=nullopt);

// add `bf` and then fix it to true
void fixBf (const Bf_ptr& bf, Mana& m); // addBf then addClause

//=================================================================================================
// Aig => Minisat
//
// add `aig` as a new state and return map from aig to minisat
map<AigVar,Var> addAig (const Aig& aig, Mana& m);
// add aig with state vars determined by `last_varmap` and return map from aig to minisat
map<AigVar,Var> addAig (const map<AigVar,Var>& last_varmap, const Aig& aig, Mana& m);

//=================================================================================================
// Stats
//
// count number of clauses that would be added for `bf`
size_t dryCountClause (const Bf_ptr& bf);
// count number of clauses that would be added for `aig`
size_t dryCountClause (const Aig& aig);
