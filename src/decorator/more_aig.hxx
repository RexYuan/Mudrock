
#pragma once

#include <cassert>

#include <queue>
using std::queue;
#include <set>
using std::set;

#include "aig.hxx"

// find cone of influence of `source` in `aig`
set<AigVar> findCOIof (const Aig& aig, const AigVar& source);
