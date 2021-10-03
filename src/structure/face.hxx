
#pragma once

#include <cassert>

#include <vector>
using std::vector;
#include <string>
using std::string;

#include <utility>
using std::move;
#include <iostream>
using std::ostream;

#include "bv.hxx"

//=================================================================================================
// Based Monotone DNF
//
struct Face
{
    Face () = default;
    Face (const Bv_ptr b);
    ~Face () = default;

    Face           (const Face& face2) = delete; // disallow copy
    Face& operator=(const Face& face2) = delete; // disallow copy

    Face           (Face&& face2); // use move
    Face& operator=(Face&& face2); // use move

    Bv_ptr                basis  () const; // get basis
    const vector<Bv_ptr>& primes () const; // get primes

    // use move to push into primes
    void push (const Bv_ptr b);

    bool size  () const; // number of primes
    bool empty () const; // if primes is empty

    bool operator () (const Bv_ptr b) const; // if a point is covered by the face

    string to_string        () const;
    string to_string_pretty () const;

private:
    Bv_ptr _basis;
    vector<Bv_ptr> _primes;
};

inline ostream& operator << (ostream& out, const Face& f) { out << f.to_string(); return out; }
