
#pragma once

#include <cassert>

#include "bf.hxx"
#include "bv.hxx"

//=================================================================================================
// Bv => Bf
//
Bf_ptr toBf (Bv* bv); // return the characteristic conjunction of `bv`; vars numbered from 0
