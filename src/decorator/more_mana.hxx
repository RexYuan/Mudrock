
#pragma once

#include <cassert>

#include <string>
using namespace std::string_literals;
using std::string;
using std::to_string;
#include <vector>
using std::vector;

#include <cmath>
using std::pow;

#include "minisat.hxx"
using namespace Minisat;

#include "bv.hxx"
#include "bf.hxx"
#include "mana.hxx"
#include "bf_to_minisat.hxx"

// test `bf` in `m`
bool sat  (const Bf_ptr& bf, Mana& m);
bool hold (const Bf_ptr& bf, Mana& m);

// set `m` vars to `valuation` according to `state_varmap` and check sat
bool evaluate (Mana& m, const Bv& valuation, const vector<Var>& state_varmap);
// set `m` vars to `valuation` according to `index_varmap` and test `bf`
bool evaluate (const Bf_ptr& bf, Mana& m, const Bv& valuation, const vector<Var>& state_varmap);

// TODO: rework tabulate with vector varmap
// enumerate minisat vars of `range` and check sat
//string tabulate (Mana& m, vector<Var> range);
