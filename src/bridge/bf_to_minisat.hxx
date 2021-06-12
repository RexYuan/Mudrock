
#pragma once

#include <cassert>

#include <optional>
using std::optional;
using std::nullopt;

#include "minisat.hxx"
using namespace Minisat;

#include "mana.hxx"
#include "bf.hxx"

//=================================================================================================
// Bf => Minisat
//
// import `bf` and return formula root; base nodes must already be in `m`
Var addBf (const Bf_ptr& bf, Mana& m, optional<Sw> swopt=nullopt, optional<bool> dvaropt=nullopt);

// add `bf` and then fix it to true
void fixBf (const Bf_ptr& bf, Mana& m); // addBf then addClause
