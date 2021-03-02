
#pragma once

#include <cassert>

#include <optional>
using std::optional; using std::nullopt;

#include "minisat.hxx"
using namespace Minisat;

#include "mana.hxx"
#include "bf.hxx"

//=================================================================================================
// Bf => Minisat
//
// import bf
Var addBf (const Bf_ptr& bf, Mana& m, optional<Sw> swopt=nullopt);

// handy queries
bool sat  (const Bf_ptr& bf, Mana& m);
bool hold (const Bf_ptr& bf, Mana& m);
