
#pragma once

#include <cassert>

#include <vector>
#include <string>

#include <utility>
#include <iostream>

#include "minisat.hxx"

#include "bv.hxx"

using std::vector;
using std::move;

using namespace Minisat;

//=================================================================================================
// M_DNF
//
struct Face
{
    Face () = default;
    Face (const Bv& b);
    ~Face () = default;
    
    Face (const Face& face2) = delete; // disallow copy
    Face& operator=(const Face& face2) = delete; // disallow copy
    
    Face (Face&& face2); // use move
    Face& operator=(Face&& face2); // use move

    const Bv& basis () const; // get basis
    const vector<Bv>& primes () const; // get primes

    // use move to push into primes
    void push (const Bv& b) = delete;
    void push (Bv&& b);

    bool operator () (const Bv& b) const; // if a point is covered by the face

    string to_string () const;
    string to_string_pretty () const;

private:
    Bv _basis;
    vector<Bv> _primes;
};

inline std::ostream& operator<<(std::ostream &out, const Face& f)
{
    out << f.to_string();
    return out;
}
