
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

#include "mana.hxx"
#include "bv.hxx"

// set minisat vars according to `valuation` corresponding to `vars` and check sat
bool evaluate (Mana& m, const Bv& valuation, const vector<Var> vars);

// enumerate minisat vars of `range` and check sat
string tabulate (Mana& m, vector<Var> range);
