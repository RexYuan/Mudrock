
#pragma once

#include <cassert>

#include <string>
using namespace std::string_literals;

#include "bf.hxx"
#include "aig.hxx"
#include "to_bf.hxx"

//=================================================================================================
// Count clauses
//
// count number of clauses that would be added for `bf`
size_t dryCountClause (const Bf_ptr& bf);
// count number of clauses that would be added for `aig`
size_t dryCountClause (const Aig& aig);
