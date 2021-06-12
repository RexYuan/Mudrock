
#pragma once

#include <cassert>

#include "bf.hxx"
#include "face.hxx"

//=================================================================================================
// Face => Bf
//
Bf_ptr toBf (const Face& face); // convert to dnf; note that vars are numbered from 0
