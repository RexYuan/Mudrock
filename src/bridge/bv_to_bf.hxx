
#pragma once

#include <cassert>

#include "bf.hxx"
#include "bv.hxx"

//=================================================================================================
// Bv => Bf
//
Bf_ptr toBf (const Bv_ptr bv); // return the characteristic conjunction of `bv`; vars numbered from 0
