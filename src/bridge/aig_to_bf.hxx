
#pragma once

#include <cassert>

#include "bf.hxx"
#include "aig.hxx"

//=================================================================================================
// Aig types => Bf
//
Bf_ptr toBf (const AigVar& aigvar);
Bf_ptr toBf (const AigLit& aiglit);
