
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

#include "bf.hxx"
#include "bv.hxx"

// if `bf` refers to the variable `v`
bool contains (const Bf_ptr& bf, int v);

// evaluate `bf` according to `val`; variables are indexed {0, ..., val.len()}
bool evaluate (const Bf_ptr& bf, const Bv& val);

// enumerate bf vars of `range` and check sat; variables are indexed {0, ..., len}
string tabulate (const Bf_ptr& bf, size_t len);
